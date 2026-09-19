__copyright__ = """
 ===========================================================================
 *              (C) Copyright 2025 - Islands-Engine - Ian Miller             *
 =============================================================================
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Affero General Public License as           *
 *  published by the Free Software Foundation, either version 3 of the       *
 *  License, or (at your option) any later version.                          *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Affero General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Affero General Public License *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ============================================================================"""
import os
import signal
import socket
import struct
import subprocess
import tempfile
import time

import pytest

ROOT = os.path.abspath(
  os.path.join(os.path.dirname(__file__), *[os.pardir] * 2))
ISLAND = os.path.join(ROOT, 'build', 'Island')
REEF = os.path.join(ROOT, 'build', 'Reef')
FIXTURES = os.path.join('tests', 'fixtures')
FOLD = 'posix'
DEADLINE = 30

pytestmark = pytest.mark.skipif(
  os.environ.get('ISLANDS_TRANSPORT', 'unix') != 'unix'
  or not (os.path.exists(ISLAND) and os.path.exists(REEF)
          and os.path.exists(os.path.join(
            ROOT, FIXTURES, 'relay', FOLD, 'libmessages.so'))),
  reason='unix build with the relations fixtures required '
         '(python3 -m make)')

FRAMES = ['--frames', '300', '--overlay', '0', '--window', 'console=0',
          '--visible', '0']


def wait(check):
  limit = time.time() + DEADLINE
  while time.time() < limit:
    if check():
      return True
    time.sleep(0.02)
  return False


def launch(binary, bundle, home, flags=()):
  return subprocess.Popen(
    [binary, '--cartridge', bundle, *flags],
    env=dict(os.environ, XDG_RUNTIME_DIR=home), cwd=ROOT,
    stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)


def rendezvous(home, service):
  return os.path.join(home, 'islands', service + '.sock')


def connect(home, service):
  link = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
  try:
    link.connect(rendezvous(home, service))
    return link
  except OSError:
    link.close()
    return None


def up(home, service):
  link = connect(home, service)
  if link is None:
    return False
  link.close()
  return True


def peer(home, service):
  link = connect(home, service)
  assert link is not None, f'{service} is not accepting'
  credentials = link.getsockopt(
    socket.SOL_SOCKET, socket.SO_PEERCRED, struct.calcsize('3i'))
  link.close()
  process, _, _ = struct.unpack('3i', credentials)
  return process


def test_owned_child_dies_with_the_island():
  with tempfile.TemporaryDirectory() as home:
    engine = launch(ISLAND, os.path.join(FIXTURES, 'owned'), home, FRAMES)
    try:
      assert wait(lambda: up(home, 'roster')), 'the owned roster came up'
      engine.wait(timeout=DEADLINE)
      assert wait(lambda: not up(home, 'roster')), \
        'the owned roster died with its parent'
    finally:
      engine.kill()
      engine.wait()


def test_owned_child_dies_with_the_reef():
  with tempfile.TemporaryDirectory() as home:
    host = launch(REEF, os.path.join(FIXTURES, 'relay'), home)
    try:
      assert wait(lambda: up(home, 'roster')), 'the owned roster came up'
      assert wait(lambda: up(home, 'messages')), 'the relay itself serves'
      host.send_signal(signal.SIGTERM)
      host.wait(timeout=DEADLINE)
      assert wait(lambda: not up(home, 'roster')), \
        'the owned roster died with the reef'
    finally:
      host.kill()
      host.wait()


def test_replace_supplants_the_running_instance():
  with tempfile.TemporaryDirectory() as home:
    stale = launch(REEF, os.path.join('cartridges', '.test', 'roster'), home)
    fresh = 0
    try:
      assert wait(lambda: up(home, 'roster')), 'the stale roster came up'
      assert peer(home, 'roster') == stale.pid
      engine = launch(ISLAND, os.path.join(FIXTURES, 'replace'), home, FRAMES)
      try:
        assert stale.wait(timeout=DEADLINE) is not None, \
          'the stale roster was ended'
        assert wait(lambda: up(home, 'roster')), 'a fresh roster took over'
        fresh = peer(home, 'roster')
        assert fresh != stale.pid
        engine.wait(timeout=DEADLINE)
        assert up(home, 'roster'), 'the released instance outlives the engine'
      finally:
        engine.kill()
        engine.wait()
    finally:
      stale.kill()
      stale.wait()
      if fresh:
        os.kill(fresh, signal.SIGTERM)


def test_requirement_cycle_refuses_the_load():
  with tempfile.TemporaryDirectory() as home:
    host = launch(REEF, os.path.join(FIXTURES, 'cycle'), home)
    try:
      output, _ = host.communicate(timeout=DEADLINE)
    finally:
      host.kill()
    assert host.returncode != 0
    assert 'cycle' in output.lower()
    assert not up(home, 'roster')
