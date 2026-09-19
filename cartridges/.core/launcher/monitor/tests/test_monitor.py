import os
import signal
import socket
import ssl
import subprocess
import tempfile
import time

import pytest

ROOT = os.path.abspath(
  os.path.join(os.path.dirname(__file__), *[os.pardir] * 5))
HOST = os.path.join(ROOT, 'build', 'Reef')
BUNDLE = os.path.join('cartridges', '.core', 'launcher', 'monitor')
SERVICE = 'monitor'
DEADLINE = 10

pytestmark = pytest.mark.skipif(
  not (os.path.exists(HOST)
       and os.path.exists(os.path.join(ROOT, BUNDLE, 'libmonitor.so'))),
  reason='reef host or monitor bundle not built (python3 -m make)')


TRANSPORT = os.environ.get('ISLANDS_TRANSPORT', 'unix')
LOOPBACKS = {'tcp': ('127.0.0.1', socket.AF_INET),
             'tcp6': ('::1', socket.AF_INET6),
             'tls': ('127.0.0.1', socket.AF_INET),
             'vsock': (socket.VMADDR_CID_LOCAL, socket.AF_VSOCK)}


def port(service):
  value = 2166136261
  for byte in service.encode():
    value = ((value ^ byte) * 16777619) & 0xFFFFFFFF
  return 49152 + value % 16383


def address(home, service):
  if TRANSPORT in LOOPBACKS:
    return (LOOPBACKS[TRANSPORT][0], port(service))
  return os.path.join(home, 'islands', service + '.sock')


def connect(addr):
  family = LOOPBACKS[TRANSPORT][1] if TRANSPORT in LOOPBACKS else socket.AF_UNIX
  link = socket.socket(family, socket.SOCK_STREAM)
  link.connect(addr)
  if TRANSPORT == 'tls':
    blind = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    blind.check_hostname = False
    blind.verify_mode = ssl.CERT_NONE
    link = blind.wrap_socket(link)
  link.settimeout(DEADLINE)
  return link


def up(addr):
  if TRANSPORT not in LOOPBACKS:
    return os.path.exists(addr)
  try:
    connect(addr).close()
    return True
  except OSError:
    return False


class Wire:

  def __init__(self, addr):
    self.link = connect(addr)
    self.data = b''

  def send(self, line):
    self.link.sendall((line + '\n').encode())

  def snapshots(self):
    entries, inside, limit = {}, False, time.time() + DEADLINE
    while True:
      while b'\n' in self.data:
        line, self.data = self.data.split(b'\n', 1)
        text = line.decode().strip()
        if text == 'begin':
          entries, inside = {}, True
        elif text == 'end' and inside:
          inside = False
          yield entries
        elif inside and text.startswith('state '):
          _, pid, state, bundle = text.split(' ', 3)
          entries[int(pid)] = (state, bundle)
      assert time.time() < limit, 'the monitor sent no complete snapshot'
      chunk = self.link.recv(4096)
      assert chunk, 'the monitor closed the stream'
      self.data += chunk

  def settle(self, predicate):
    for view in self.snapshots():
      if predicate(view):
        return view

  def close(self):
    self.link.close()


@pytest.fixture
def monitor():
  home = tempfile.mkdtemp(prefix='monitor-', dir='/tmp')
  addr = address(home, SERVICE)
  host = subprocess.Popen(
    [HOST, '--cartridge', BUNDLE],
    env={**os.environ, 'XDG_RUNTIME_DIR': home},
    stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
  limit = time.time() + DEADLINE
  while not up(addr):
    assert host.poll() is None, 'the monitor exited before binding'
    assert time.time() < limit, 'the monitor never came up'
    time.sleep(0.02)
  yield addr
  host.send_signal(signal.SIGTERM)
  assert host.wait(timeout=DEADLINE) == 0, 'the host did not exit cleanly'
  assert not up(addr), 'the monitor left its address bound'


@pytest.fixture
def child():
  proc = subprocess.Popen(['sleep', str(DEADLINE * 3)])
  yield proc
  if proc.poll() is None:
    proc.terminate()
  proc.wait()


def test_empty_roster_answers_bare_snapshot(monitor):
  peer = Wire(monitor)
  peer.send('poll')
  assert peer.settle(lambda view: True) == {}
  peer.close()


def test_watch_lists_the_child_running(monitor, child):
  peer = Wire(monitor)
  peer.send(f'watch {child.pid} castaway')
  view = peer.settle(lambda view: child.pid in view)
  assert view[child.pid] == ('running', 'castaway')
  peer.close()


def test_reaped_child_reads_stopped(monitor, child):
  peer = Wire(monitor)
  peer.send(f'watch {child.pid} castaway')
  peer.settle(lambda view: view.get(child.pid) == ('running', 'castaway'))
  child.terminate()
  child.wait()
  peer.send('poll')
  view = peer.settle(lambda view: view.get(child.pid, ('', ''))[0] == 'stopped')
  assert view[child.pid] == ('stopped', 'castaway')
  peer.close()


def test_forget_drops_the_row(monitor, child):
  peer = Wire(monitor)
  peer.send(f'watch {child.pid} castaway')
  peer.settle(lambda view: child.pid in view)
  peer.send(f'forget {child.pid}')
  assert child.pid not in peer.settle(lambda view: child.pid not in view)
  peer.close()
