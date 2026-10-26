import http.client
import socket
import subprocess
import sys
import urllib.parse

QUERY = 'status=SUCCESS&connection_id=decoy'
WORD = b'connected'
OVERSIZE = 9 << 10


def ask(where, target, method='GET'):
  link = http.client.HTTPConnection(where[0], where[1], timeout=20)
  link.request(method, target)
  answer = link.getresponse()
  body = answer.read()
  headers = {name.lower(): value for name, value in answer.getheaders()}
  link.close()
  return answer.status, headers, body


def clauses(where, path, run):
  def missing():
    status, _, body = ask(where, '/elsewhere?' + QUERY)
    return None if (status, body) == (404, b'') else f'{status} {body!r}'

  def posted():
    status, headers, body = ask(where, path, 'POST')
    if (status, body) != (405, b''):
      return f'{status} {body!r}'
    return None if headers.get('allow') == 'GET' else 'no Allow line'

  def oversized():
    with socket.create_connection(where, timeout=20) as link:
      try:
        link.sendall(b'GET ' + path.encode() + b' HTTP/1.1\r\nX: '
                     + b'a' * OVERSIZE)
        heard = link.recv(1024)
      except (ConnectionResetError, BrokenPipeError):
        heard = b''
    return None if heard == b'' else f'answered {heard[:40]!r}'

  def asked():
    status, headers, body = ask(where, f'{path}?{QUERY}')
    if (status, body) != (200, WORD):
      return f'{status} {body!r}'
    if headers.get('content-type') != 'text/plain':
      return f'type {headers.get("content-type")!r}'
    if headers.get('cache-control') != 'no-store':
      return f'may be stored: {headers.get("cache-control")!r}'
    if headers.get('connection') != 'close':
      return f'connection {headers.get("connection")!r}'
    heard = run.stdout.readline().split(' ', 1)
    if heard != ['query', QUERY + '\n']:
      return f'the fixture read {heard!r}'
    return None

  def closed():
    try:
      status, _, _ = ask(where, f'{path}?{QUERY}')
    except ConnectionRefusedError:
      return None
    return f'a second ask answered {status}'

  return [('missing', missing), ('posted', posted), ('oversized', oversized),
          ('asked', asked), ('closed', closed)]


def declared(fixture):
  run = subprocess.Popen([fixture, 'declared'], stdout=subprocess.PIPE,
                         text=True)
  spoken = run.stdout.readline().split()
  if len(spoken) != 2 or spoken[0] != 'address':
    print(f'FAIL bound: the fixture said {spoken!r}')
    run.kill()
    return 1
  address = urllib.parse.urlsplit(spoken[1])
  if address.scheme != 'http' or address.hostname != '127.0.0.1':
    print(f'FAIL bound: {spoken[1]!r} is not plain http on 127.0.0.1')
    run.kill()
    return 1
  print('PASS bound')
  where = (address.hostname, address.port)
  for name, check in clauses(where, address.path, run):
    reason = check()
    print(f'{"PASS" if reason is None else "FAIL"} {name}'
          + (f': {reason}' if reason else ''))
    if reason is not None:
      run.kill()
      return 1
  return 1 if run.wait(timeout=60) else 0


def undeclared(fixture):
  verdict = subprocess.run([fixture, 'undeclared'], timeout=60).returncode
  print('PASS undeclared' if verdict == 0 else 'FAIL undeclared: open bound')
  return verdict


def main():
  fixture = sys.argv[1]
  return declared(fixture) or undeclared(fixture)


if __name__ == '__main__':
  sys.exit(main())
