import base64
import hashlib
import os
import socketserver
import ssl
import struct
import subprocess
import sys
import threading

GUID = b'258EAFA5-E914-47DA-95CA-C5AB0DC85B11'
TEXT, CONTINUATION, CLOSE, PING, PONG = 0x1, 0x0, 0x8, 0x9, 0xA
PAPER = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                     'fixtures', 'trusted')
CERTIFICATE = os.path.join(PAPER, 'assets', 'gateway.crt')
seen = {'upgrades': 0, 'faults': [], 'closes': 0}


def accept(key):
  return base64.b64encode(hashlib.sha1(key + GUID).digest())


def frame(opcode, payload, fin=True):
  head = bytes([(0x80 if fin else 0) | opcode])
  size = len(payload)
  if size < 126:
    return head + bytes([size]) + payload
  if size < 1 << 16:
    return head + bytes([126]) + struct.pack('!H', size) + payload
  return head + bytes([127]) + struct.pack('!Q', size) + payload


def exact(link, size):
  data = b''
  while len(data) < size:
    page = link.recv(size - len(data))
    if not page:
      raise ConnectionError('closed')
    data += page
  return data


def read(link):
  first, second = exact(link, 2)
  size = second & 0x7F
  if size == 126:
    size = struct.unpack('!H', exact(link, 2))[0]
  elif size == 127:
    size = struct.unpack('!Q', exact(link, 8))[0]
  if not second & 0x80:
    seen['faults'].append('unmasked frame')
  key = exact(link, 4) if second & 0x80 else b'\0\0\0\0'
  body = bytes(b ^ key[i % 4] for i, b in enumerate(exact(link, size)))
  return first & 0x0F, body


class Origin(socketserver.BaseRequestHandler):
  def handle(self):
    head = b''
    while b'\r\n\r\n' not in head:
      head += self.request.recv(4096)
    lines = head.decode().split('\r\n')
    fields = dict(l.split(': ', 1) for l in lines[1:] if ': ' in l)
    key = fields.get('Sec-WebSocket-Key', '').encode()
    forged = lines[0].split()[1].startswith('/forged')
    seen['upgrades'] += 1
    self.request.sendall(
      b'HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n'
      b'Connection: Upgrade\r\nSec-WebSocket-Accept: '
      + accept(b'wrong' if forged else key) + b'\r\n\r\n')
    while not forged and self.answer(*read(self.request)):
      pass

  def answer(self, opcode, body):
    send = self.request.sendall
    if opcode == CLOSE or body == b'close':
      send(frame(CLOSE, b''))
      seen['closes'] += opcode == CLOSE or read(self.request)[0] == CLOSE
      return False
    if body == b'fragment':
      send(frame(TEXT, b'frag', fin=False) + frame(CONTINUATION, b'mented'))
    elif body == b'ping':
      send(frame(PING, b'beat'))
      pong = read(self.request)
      send(frame(TEXT, b'pong beat' if pong == (PONG, b'beat') else b'bad'))
    else:
      send(frame(TEXT, body))
    return True


def served(secured):
  server = socketserver.ThreadingTCPServer(('127.0.0.1', 0), Origin)
  server.daemon_threads = True
  if secured:
    paper = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    paper.load_cert_chain(CERTIFICATE, os.path.join(PAPER, 'gateway.key'))
    server.socket = paper.wrap_socket(server.socket, server_side=True)
  threading.Thread(target=server.serve_forever, daemon=True).start()
  return str(server.server_address[1])


def main():
  ports = [served(False), served(True)]
  verdict = subprocess.run([sys.argv[1], *ports, CERTIFICATE],
                           timeout=120).returncode
  checks = [('upgrades', seen['upgrades'] == 3), ('masked', not seen['faults']),
            ('answered', seen['closes'] == 2)]
  for name, good in checks:
    print(f'{"PASS" if good else "FAIL"} origin {name}')
  return verdict or int(not all(good for _, good in checks))


if __name__ == '__main__':
  sys.exit(main())
