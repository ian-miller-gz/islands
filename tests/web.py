import sys

import driver

STAGES = {
  'gated': [
    ('web', 'usage: web <url>'),
    ('web https://api.example.com/data',
     'web transfer refused: no backend compiled (SR_WEB_BACKEND: SR_NONE); '
     'requested https://api.example.com/data'),
    ('web http://elsewhere.test/data',
     'web transfer refused: http://elsewhere.test is not in the declared '
     'allowlist; requested http://elsewhere.test/data'),
  ],
  'bare': [
    ('web', 'usage: web <url>'),
    ('web https://api.example.com/data',
     'web transfer refused: no web reach declared (the manifest web: key or '
     '--web); requested https://api.example.com/data'),
  ],
}


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
  path, mode = sys.argv[1], sys.argv[2]
  link = driver.connect(path)
  link.settimeout(driver.DEADLINE)
  pending = b''
  for command, expect in STAGES[mode]:
    link.sendall((command + '\n').encode())
    taken, pending = seek(link, pending, expect)
    sys.stdout.write(taken.decode())
  link.sendall(b'quit\n')
  sys.stdout.write((pending + driver.drain(link)).decode())


if __name__ == '__main__':
  main()
