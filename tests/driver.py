import os
import signal
import socket
import ssl
import subprocess
import tempfile
import threading
import time

import islands

DEADLINE = 30

TRANSPORT = islands.stream(os.path.dirname(os.path.abspath(__file__)))

LOOPBACKS = {'tcp': (socket.AF_INET, '127.0.0.1', '', ':'),
             'tcp6': (socket.AF_INET6, '::1', '[', ']:'),
             'tls': (socket.AF_INET, '127.0.0.1', '', ':')}


def until(check, what):
  limit = time.time() + DEADLINE
  while not check():
    if time.time() > limit:
      raise TimeoutError(f'{what} never happened')
    time.sleep(0.1)


def home():
  return tempfile.mkdtemp(prefix='islands-', dir='/tmp')


def port(service):
  value = 2166136261
  for byte in service.encode():
    value = ((value ^ byte) * 16777619) & 0xFFFFFFFF
  return 49152 + value % 16383


def spell(service):
  _, host, opened, shut = LOOPBACKS[TRANSPORT]
  return f'{opened}{host}{shut}{port(service)}'


def rendezvous(directory, service):
  if TRANSPORT == 'unix':
    return os.path.join(directory, 'islands', service + '.sock')
  return spell(service)


def endpoint():
  path = tempfile.mktemp(prefix='islands-', suffix='.sock', dir='/tmp')
  return path if TRANSPORT == 'unix' else spell(os.path.basename(path))


def attach(where):
  if TRANSPORT == 'unix':
    link = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
  else:
    link = socket.socket(LOOPBACKS[TRANSPORT][0], socket.SOCK_STREAM)
  try:
    if TRANSPORT == 'unix':
      link.connect(where)
    else:
      _, _, opened, shut = LOOPBACKS[TRANSPORT]
      cut = where.rfind(shut)
      link.connect((where[len(opened):cut], int(where[cut + len(shut):])))
      if TRANSPORT == 'tls':
        blind = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        blind.check_hostname = False
        blind.verify_mode = ssl.CERT_NONE
        link = blind.wrap_socket(link)
  except OSError:
    link.close()
    raise
  return link


def connect(where):
  limit = time.time() + DEADLINE
  while True:
    try:
      return attach(where)
    except OSError:
      if time.time() > limit:
        raise
      time.sleep(0.1)


def up(where):
  if TRANSPORT == 'unix':
    return os.path.exists(where)
  try:
    attach(where).close()
    return True
  except OSError:
    return False


def seek(link, pending, text):
  taken = b''
  while True:
    while b'\n' in pending:
      line, pending = pending.split(b'\n', 1)
      taken += line + b'\n'
      if text.encode() in line:
        return taken, pending
    chunk = link.recv(4096)
    assert chunk, f'the engine closed before {text!r}'
    pending += chunk


def drain(link):
  data = b''
  while chunk := link.recv(4096):
    data += chunk
  return data


def launch(command, directory, **options):
  return subprocess.Popen(
    command, env={**os.environ, 'XDG_RUNTIME_DIR': directory}, **options)


def host(binary, bundle, directory):
  return launch(
    [os.path.join(os.path.dirname(binary), 'Reef'), '--cartridge', bundle],
    directory, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)


class Reader:

  def __init__(self, pipe):
    self.lines = []
    threading.Thread(target=self._drain, args=(pipe,), daemon=True).start()

  def _drain(self, pipe):
    for line in pipe:
      self.lines.append(line)

  def has(self, text):
    return any(text in line for line in self.lines)

  def count(self, text):
    return sum(text in line for line in self.lines)

  def line(self, text):
    return any(line.strip() == text for line in self.lines)


def tell(process, line):
  process.stdin.write(line + '\n')
  process.stdin.flush()


def alive(pid):
  try:
    os.kill(pid, 0)
  except OSError:
    return False
  return True


def hosts(directory):
  found = []
  for pid in filter(str.isdigit, os.listdir('/proc')):
    try:
      with open(f'/proc/{pid}/cmdline', 'rb') as file:
        command = file.read().split(b'\0')
      with open(f'/proc/{pid}/environ', 'rb') as file:
        environ = file.read().split(b'\0')
    except OSError:
      continue
    if command and b'Reef' in command[0] and \
        f'XDG_RUNTIME_DIR={directory}'.encode() in environ:
      found.append(int(pid))
  return found


def down(*processes):
  for process in processes:
    if process and process.poll() is None:
      process.kill()
      process.wait()


def reap(directory):
  for pid in hosts(directory):
    os.kill(pid, signal.SIGTERM)
