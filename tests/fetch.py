import http.server
import sys
import threading

import driver

RESOURCE = b'hello, islands\nthe body past the first line\n'
CHUNKS = [b'chunked ', b'islands', b'\nand a second line\n']


class Origin(http.server.BaseHTTPRequestHandler):

  def do_GET(self):
    if self.path == '/plain':
      self.send_response(200)
      self.send_header('Content-Length', str(len(RESOURCE)))
      self.end_headers()
      self.wfile.write(RESOURCE)
    elif self.path == '/chunked':
      self.send_response(200)
      self.send_header('Transfer-Encoding', 'chunked')
      self.end_headers()
      for chunk in CHUNKS:
        self.wfile.write(b'%X\r\n%s\r\n' % (len(chunk), chunk))
      self.wfile.write(b'0\r\n\r\n')
    elif self.path == '/moved':
      self.send_response(302)
      self.send_header('Location', '/plain')
      self.end_headers()
    elif self.path == '/offsite':
      self.send_response(302)
      self.send_header('Location', 'http://elsewhere.test/data')
      self.end_headers()
    else:
      self.send_response(404)
      self.send_header('Content-Length', '0')
      self.end_headers()

  def log_message(self, *arguments):
    pass


def staged(port):
  origin = f'http://127.0.0.1:{port}'
  return [
    (f'web {origin}/plain', 'web 200 (44 bytes): hello, islands'),
    (f'web {origin}/chunked', 'web 200 (34 bytes): chunked islands'),
    (f'web {origin}/moved', 'web 200 (44 bytes): hello, islands'),
    (f'web {origin}/missing', 'web 404 (0 bytes).'),
    (f'web {origin}/offsite',
     'web transfer refused: http://elsewhere.test is not in the declared '
     'allowlist; requested http://elsewhere.test/data'),
  ]


def main():
  server = http.server.HTTPServer(('127.0.0.1', 0), Origin)
  threading.Thread(target=server.serve_forever, daemon=True).start()
  link = driver.connect(sys.argv[1])
  link.settimeout(driver.DEADLINE)
  pending = b''
  for command, expect in staged(server.server_address[1]):
    link.sendall((command + '\n').encode())
    taken, pending = driver.seek(link, pending, expect)
    sys.stdout.write(taken.decode())
  link.sendall(b'quit\n')
  sys.stdout.write((pending + driver.drain(link)).decode())
  server.shutdown()


if __name__ == '__main__':
  main()
