import http.server
import os
import ssl
import sys
import threading

import driver

SERVICE = 'islands-trusted-origin'
BUNDLE = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                      'fixtures', 'trusted')
RESOURCE = b'the gateway answers\nand a second line\n'


class Origin(http.server.BaseHTTPRequestHandler):

  def do_GET(self):
    self.send_response(200)
    self.send_header('Content-Length', str(len(RESOURCE)))
    self.end_headers()
    self.wfile.write(RESOURCE)

  def log_message(self, *arguments):
    pass


def secured(port):
  server = http.server.HTTPServer(('127.0.0.1', port), Origin)
  paper = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
  paper.load_cert_chain(os.path.join(BUNDLE, 'assets', 'gateway.crt'),
                        os.path.join(BUNDLE, 'gateway.key'))
  server.socket = paper.wrap_socket(server.socket, server_side=True)
  return server


def staged(port):
  return [
    (f'web https://localhost:{port}/paper',
     'web 200 (38 bytes): the gateway answers'),
    (f'web https://127.0.0.1:{port}/paper',
     f'web transfer failed: could not reach 127.0.0.1:{port}; '
     f'requested https://127.0.0.1:{port}/paper'),
  ]


def main():
  port = driver.port(SERVICE)
  server = secured(port)
  threading.Thread(target=server.serve_forever, daemon=True).start()
  link = driver.connect(sys.argv[1])
  link.settimeout(driver.DEADLINE)
  pending = b''
  for command, expect in staged(port):
    link.sendall((command + '\n').encode())
    taken, pending = driver.seek(link, pending, expect)
    sys.stdout.write(taken.decode())
  link.sendall(b'quit\n')
  sys.stdout.write((pending + driver.drain(link)).decode())
  server.shutdown()


if __name__ == '__main__':
  main()
