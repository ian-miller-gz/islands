import base64
import http.client
import os
import socket
import ssl
import subprocess
import sys

READER = ('wife', 'kitchen')
GUEST = ('hotel', 'bar')


def reserve():
  keep = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  keep.bind(('127.0.0.1', 0))
  port = keep.getsockname()[1]
  keep.close()
  return port


def credential(who):
  raw = f'{who[0]}:{who[1]}'.encode()
  return {'Authorization': 'Basic ' + base64.b64encode(raw).decode()}


def ask(where, path, who=None, method='GET'):
  trust = ssl.create_default_context(cafile=os.path.join(HOME, 'reader.crt'))
  link = http.client.HTTPSConnection(
    where[0], where[1], context=trust, timeout=20)
  link.request(method, path, headers=credential(who) if who else {})
  answer = link.getresponse()
  body = answer.read()
  headers = {name.lower(): value for name, value in answer.getheaders()}
  link.close()
  return answer.status, headers, body


def clauses(where):
  def challenged():
    status, headers, body = ask(where, '/')
    if status != 401:
      return f'an ask with no credential answered {status}, not 401'
    if not headers.get('www-authenticate', '').startswith('Basic realm='):
      return f'no Basic challenge: {headers.get("www-authenticate")!r}'
    return None if not body else 'a refusal carried a body'

  def refused():
    status, _, _ = ask(where, '/', ('wife', 'wrong'))
    return None if status == 401 else f'a wrong secret answered {status}'

  def answered():
    status, headers, body = ask(where, '/', READER)
    if status != 200:
      return f'the granted reader was answered {status}'
    if body != b'reader: wife\n':
      return f'the answer did not name its reader: {body!r}'
    if headers.get('cache-control') != 'no-store':
      return f'an answer may be stored: {headers.get("cache-control")!r}'
    return None

  def targeted():
    status, _, body = ask(where, '/where?q=x%20y', READER)
    want = b'path=/where query=q=x%20y\n'
    return None if (status, body) == (200, want) else f'{status} {body!r}'

  def headed():
    status, headers, body = ask(where, '/', READER, method='HEAD')
    if status != 200 or body:
      return f'HEAD answered {status} with {len(body)} body bytes'
    return None if headers.get('content-length') == '13' else 'no length'

  def written():
    status, headers, body = ask(where, '/', READER, method='POST')
    if status != 405:
      return f'a write answered {status}, not 405'
    if headers.get('allow') != 'GET, HEAD':
      return f'no Allow line: {headers.get("allow")!r}'
    return None if not body else 'the answering function ran for a POST'

  def unserved():
    status, _, body = ask(where, '/missing', READER)
    want = b'no such page: /missing\n'
    return None if (status, body) == (404, want) else f'{status} {body!r}'

  def revoked():
    status, _, _ = ask(where, '/revoke', GUEST)
    if status != 200:
      return f'the guest could not reach /revoke: {status}'
    status, _, _ = ask(where, '/', GUEST)
    return None if status == 401 else f'a revoked reader answered {status}'

  def closed():
    status, _, _ = ask(where, '/done', READER)
    return None if status == 200 else f'/done answered {status}'

  return [('challenged', challenged), ('refused', refused),
          ('answered', answered), ('targeted', targeted), ('headed', headed),
          ('written', written), ('unserved', unserved), ('revoked', revoked),
          ('closed', closed)]


def main():
  fixture, arm = sys.argv[1], sys.argv[2]
  wire = f'127.0.0.1:{reserve()}'
  run = subprocess.Popen(
    [fixture, arm, wire, HOME + os.sep], stdout=subprocess.PIPE, text=True)
  if arm == 'dark':
    verdict = run.wait(timeout=60)
    print('PASS dark' if verdict == 0 else 'FAIL dark: open did not refuse')
    return 0 if verdict == 0 else 1
  spoken = run.stdout.readline().split()
  if len(spoken) != 2 or spoken[0] != 'serving':
    print(f'FAIL bound: the fixture said {spoken!r}')
    run.kill()
    return 1
  address, port = spoken[1].rsplit(':', 1)
  print('PASS bound')
  for name, check in clauses((address, int(port))):
    reason = check()
    print(f'{"PASS" if reason is None else "FAIL"} {name}'
          + (f': {reason}' if reason else ''))
    if reason is not None:
      run.kill()
      return 1
  return 1 if run.wait(timeout=60) else 0


HOME = os.path.join(
  os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
  'src', 'test', 'reader')

if __name__ == '__main__':
  sys.exit(main())
