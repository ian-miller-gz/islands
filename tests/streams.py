import os
import socket
import subprocess
import sys
import tempfile
import threading


def echo(ear):
  link, _ = ear.accept()
  with link:
    data = b''
    while not data.endswith(b'\n'):
      chunk = link.recv(4096)
      if not chunk:
        return
      data += chunk
    link.sendall(data)


def reserve():
  keep = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  keep.bind(('127.0.0.1', 0))
  port = keep.getsockname()[1]
  keep.close()
  return port


def main():
  binary = sys.argv[1]
  secondary = sys.argv[2] if len(sys.argv) > 2 else 'tcp'
  home = tempfile.mkdtemp(prefix='streams-')
  path = os.path.join(home, 'echo.sock')
  unix = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
  unix.bind(path)
  unix.listen(1)
  threading.Thread(target=echo, args=(unix,), daemon=True).start()
  loop = None
  if secondary == 'tcp':
    loop = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    loop.bind(('127.0.0.1', 0))
    port = loop.getsockname()[1]
    loop.listen(1)
    threading.Thread(target=echo, args=(loop,), daemon=True).start()
  else:
    port = reserve()
  try:
    return subprocess.run([binary, path, str(port)], timeout=30).returncode
  finally:
    unix.close()
    if loop is not None:
      loop.close()
    if os.path.exists(path):
      os.unlink(path)


if __name__ == '__main__':
  sys.exit(main())
