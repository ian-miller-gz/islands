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
BUNDLE = os.path.join('cartridges', '.test', 'messages')
FOLD = 'posix'
DEADLINE = 10

pytestmark = pytest.mark.skipif(
  not (os.path.exists(HOST)
       and os.path.exists(os.path.join(ROOT, BUNDLE, FOLD,
                                      'libmessages.so'))),
  reason='reef host or messages bundle not built (python3 -m make)')


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


def subscribe(path, topic):
  link = connect(path)
  link.sendall(f'subscribe\t{topic}\n'.encode())
  return link


def publish(link, topic, payload):
  link.sendall(f'publish\t{topic}\t{payload}\n'.encode())


def line(link):
  data = b''
  while b'\n' not in data:
    chunk = link.recv(4096)
    if not chunk:
      return None
    data += chunk
  return data.split(b'\n', 1)[0].decode()


def deliver(publisher, subscriber, topic, payload):
  limit = time.time() + DEADLINE
  subscriber.settimeout(0.2)
  try:
    while time.time() < limit:
      publish(publisher, topic, payload)
      try:
        return line(subscriber)
      except TimeoutError:
        continue
    raise TimeoutError('the delivery never happened')
  finally:
    subscriber.settimeout(DEADLINE)


@pytest.fixture(params=sorted(THREADED))
def daemon(request):
  home = tempfile.mkdtemp(prefix='messages-', dir='/tmp')
  addr = address(home, 'messages')
  host = subprocess.Popen(
    [HOST, '--cartridge', BUNDLE] + THREADED[request.param],
    env={**os.environ, 'XDG_RUNTIME_DIR': home},
    stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
  limit = time.time() + DEADLINE
  while not up(addr):
    assert time.time() < limit, 'the daemon never came up'
    time.sleep(0.02)
  yield addr
  host.send_signal(signal.SIGTERM)
  assert host.wait(timeout=DEADLINE) == 0, 'the host did not exit cleanly'
  assert not up(addr), 'the daemon left its address bound'


@pytest.mark.parametrize('mode', sorted(THREADED))
def test_interrupt_ends_the_run(mode):
  home = tempfile.mkdtemp(prefix='messages-', dir='/tmp')
  addr = address(home, 'messages')
  host = subprocess.Popen(
    [HOST, '--cartridge', BUNDLE] + THREADED[mode],
    env={**os.environ, 'XDG_RUNTIME_DIR': home},
    stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
  limit = time.time() + DEADLINE
  while not up(addr):
    assert time.time() < limit, 'the daemon never came up'
    time.sleep(0.02)
  host.send_signal(signal.SIGINT)
  assert host.wait(timeout=DEADLINE) == 0, 'the host did not exit cleanly'
  assert not up(addr), 'the daemon left its address bound'


def test_publish_reaches_subscriber(daemon):
  follower = subscribe(daemon, 'frames')
  speaker = connect(daemon)
  assert deliver(speaker, follower, 'frames', '1\t0.5') == 'frames\t1\t0.5'
  follower.close()
  speaker.close()


def test_topics_filter(daemon):
  frames = subscribe(daemon, 'frames')
  other = subscribe(daemon, 'other')
  speaker = connect(daemon)
  deliver(speaker, frames, 'frames', 'x')
  assert deliver(speaker, other, 'other', 'y') == 'other\ty'
  for link in (frames, other, speaker):
    link.close()


def test_junk_never_costs_the_slot(daemon):
  follower = subscribe(daemon, 'frames')
  speaker = connect(daemon)
  speaker.sendall(b'nonsense line\npublish\ttopiconly\n\nsubscribe\t\n')
  assert deliver(speaker, follower, 'frames', 'still here')
  follower.close()
  speaker.close()


def test_closed_subscriber_is_reaped(daemon):
  gone = subscribe(daemon, 'frames')
  stays = subscribe(daemon, 'frames')
  speaker = connect(daemon)
  deliver(speaker, gone, 'frames', 'warmup')
  gone.close()
  assert deliver(speaker, stays, 'frames', 'after')
  stays.close()
  speaker.close()


def test_stalled_subscriber_is_dropped(daemon):
  stalled = subscribe(daemon, 'frames')
  speaker = connect(daemon)
  deliver(speaker, stalled, 'frames', 'warmup')
  for _ in range(8):
    publish(speaker, 'frames', 'x' * (1 << 20))
  while line(stalled) is not None:
    pass
  fresh = subscribe(daemon, 'frames')
  assert deliver(speaker, fresh, 'frames', 'alive')
  fresh.close()
  speaker.close()
  stalled.close()


DEAF = os.path.join('tests', 'fixtures', 'deaf')


def test_deaf_manifest_refuses():
  assert os.path.exists(os.path.join(ROOT, DEAF, FOLD, 'libmessages.so')), \
    'deaf fixture not populated (python3 -m make)'
  home = tempfile.mkdtemp(prefix='deaf-', dir='/tmp')
  host = subprocess.Popen(
    [HOST, '--cartridge', DEAF],
    env={**os.environ, 'XDG_RUNTIME_DIR': home},
    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
  output, _ = host.communicate(timeout=DEADLINE)
  assert host.returncode != 0, 'the refusal must end the run'
  assert 'listen' in output.lower(), 'the refusal must name the listen key'
