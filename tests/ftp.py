import socket
import socketserver
import sys
import threading

import driver

GREETING = b'ahoy from the archive\nand a second line\n'
LEGACY = b'the older origin answers too\n'
DEPOSIT = 'a resource the session stored'

RESOURCES = {'/greeting.txt': GREETING, '/legacy.txt': LEGACY}


class Origin(socketserver.StreamRequestHandler):

  extended = True
  decoy = '127.0.0.1'
  timeout = driver.DEADLINE

  def say(self, text):
    self.wfile.write((text + '\r\n').encode())
    self.wfile.flush()

  def passive(self):
    self.listener = socket.socket()
    self.listener.bind(('127.0.0.1', 0))
    self.listener.listen(1)
    self.listener.settimeout(driver.DEADLINE)
    return self.listener.getsockname()[1]

  def move(self, word, path):
    if word == 'RETR' and path not in RESOURCES:
      self.listener.close()
      self.say('550 no such resource')
      return
    self.say('150 opening the data connection')
    link, _ = self.listener.accept()
    self.listener.close()
    if word == 'RETR':
      link.sendall(RESOURCES[path])
    else:
      taken = b''
      while chunk := link.recv(4096):
        taken += chunk
      RESOURCES[path] = taken
    link.close()
    self.say('226 transfer complete')

  def handle(self):
    self.say('220-islands test origin')
    self.say('220 ready')
    while line := self.rfile.readline():
      word, _, rest = line.decode().strip().partition(' ')
      word = word.upper()
      if word in ('USER',):
        self.say('331 send the password')
      elif word in ('PASS',):
        self.say('230 logged in')
      elif word in ('TYPE',):
        self.say('200 binary it is')
      elif word == 'EPSV' and self.extended:
        self.say(f'229 entering extended passive mode (|||{self.passive()}|)')
      elif word == 'PASV':
        port = self.passive()
        octets = self.decoy.replace('.', ',')
        self.say(
          f'227 entering passive mode ({octets},{port // 256},{port % 256})')
      elif word in ('RETR', 'STOR'):
        self.move(word, rest)
      elif word == 'QUIT':
        self.say('221 goodbye')
        return
      else:
        self.say('502 that command is not implemented')


class Legacy(Origin):

  extended = False
  decoy = '203.0.113.9'


def serve(handler):
  server = socketserver.ThreadingTCPServer(('127.0.0.1', 0), handler)
  server.daemon_threads = True
  threading.Thread(target=server.serve_forever, daemon=True).start()
  return server


def staged(modern, older):
  archive = f'ftp://127.0.0.1:{modern}'
  return [
    (f'{archive}/greeting.txt', 'web 226 (40 bytes): ahoy from the archive'),
    (f'{archive}/missing.txt', 'web 550 (0 bytes).'),
    (f'ftp://127.0.0.1:{older}/legacy.txt',
     'web 226 (29 bytes): the older origin answers too'),
    (f'{archive}/deposit.txt {DEPOSIT}', 'web 226 (0 bytes).'),
    (f'{archive}/deposit.txt', f'web 226 (29 bytes): {DEPOSIT}'),
    ('ftp://elsewhere.test/data',
     'web transfer refused: ftp://elsewhere.test is not in the declared '
     'allowlist; requested ftp://elsewhere.test/data'),
    ('http://127.0.0.1/data',
     'web transfer refused: http://127.0.0.1 is not in the declared '
     'allowlist; requested http://127.0.0.1/data'),
  ]


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


def main():
  modern, older = serve(Origin), serve(Legacy)
  link = driver.connect(sys.argv[1])
  link.settimeout(driver.DEADLINE)
  pending = b''
  for target, expect in staged(
      modern.server_address[1], older.server_address[1]):
    link.sendall((f'web {target}\n').encode())
    taken, pending = seek(link, pending, expect)
    sys.stdout.write(taken.decode())
  link.sendall(b'quit\n')
  sys.stdout.write((pending + driver.drain(link)).decode())
  modern.shutdown()
  older.shutdown()


if __name__ == '__main__':
  main()
