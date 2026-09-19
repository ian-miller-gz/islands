import os
import signal
import socket
import ssl
import subprocess
import tempfile
import time

import pytest

ROOT = os.path.abspath(
  os.path.join(os.path.dirname(__file__), *[os.pardir] * 4))
HOST = os.path.join(ROOT, 'Reef')
BUNDLE = os.path.join('cartridges', '.test', 'roster')
FOLD = 'posix'
DEADLINE = 10

pytestmark = pytest.mark.skipif(
  not (os.path.exists(HOST)
       and os.path.exists(os.path.join(ROOT, BUNDLE, FOLD, 'libroster.so'))),
  reason='reef host or roster bundle not built (python3 -m make)')


def wait(check):
  limit = time.time() + DEADLINE
  while time.time() < limit:
    if check():
      return True
    time.sleep(0.02)
  return False


TRANSPORT = os.environ.get('ISLANDS_TRANSPORT', 'unix')
THREADED = {'single': [], 'threaded': ['--threads', '1']}
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
  return link


def up(addr):
  if TRANSPORT not in LOOPBACKS:
    return os.path.exists(addr)
  try:
    connect(addr).close()
    return True
  except OSError:
    return False


def query(path):
  with connect(path) as link:
    link.sendall(b'list\n')
    data = b''
    while chunk := link.recv(4096):
      data += chunk
  return [line.split('\t') for line in data.decode().splitlines() if line]


def register(path, entry):
  link = connect(path)
  link.sendall(('\t'.join(('hello',) + entry) + '\n').encode())
  return link


@pytest.fixture(params=sorted(THREADED))
def daemon(request):
  home = tempfile.mkdtemp(prefix='roster-', dir='/tmp')
  addr = address(home, 'roster')
  host = subprocess.Popen(
    [HOST, '--cartridge', BUNDLE] + THREADED[request.param],
    env={**os.environ, 'XDG_RUNTIME_DIR': home},
    stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
  assert wait(lambda: up(addr)), 'the daemon never came up'
  yield addr
  host.send_signal(signal.SIGTERM)
  assert host.wait(timeout=DEADLINE) == 0, 'the host did not exit cleanly'
  assert not up(addr), 'the daemon left its address bound'


@pytest.mark.parametrize('mode', sorted(THREADED))
def test_interrupt_ends_the_run(mode):
  home = tempfile.mkdtemp(prefix='roster-', dir='/tmp')
  addr = address(home, 'roster')
  host = subprocess.Popen(
    [HOST, '--cartridge', BUNDLE] + THREADED[mode],
    env={**os.environ, 'XDG_RUNTIME_DIR': home},
    stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
  assert wait(lambda: up(addr)), 'the daemon never came up'
  host.send_signal(signal.SIGINT)
  assert host.wait(timeout=DEADLINE) == 0, 'the host did not exit cleanly'
  assert not up(addr), 'the daemon left its address bound'


def test_register_list_deregister(daemon):
  entry = ('123', 'Islands', 'cartridges/.test/apps/world', '/tmp/123.sock')
  link = register(daemon, entry)
  assert wait(lambda: query(daemon) == [list(entry)])
  link.close()
  assert wait(lambda: query(daemon) == [])


def test_crash_reaps(daemon):
  one = ('1', 'Islands', 'cartridges/.test/apps/world', '/tmp/1.sock')
  two = ('2', 'Islands', 'none', '/tmp/2.sock')
  first = register(daemon, one)
  second = register(daemon, two)
  assert wait(lambda: len(query(daemon)) == 2)
  first.close()
  assert wait(lambda: query(daemon) == [list(two)])
  second.close()
  assert wait(lambda: query(daemon) == [])


def test_junk_lines_never_register(daemon):
  link = connect(daemon)
  link.sendall(b'hello\ttoo\tfew\n\nnonsense line\n')
  time.sleep(0.2)
  assert query(daemon) == []
  link.close()
