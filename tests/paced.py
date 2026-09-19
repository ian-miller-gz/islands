import os
import sys

import driver

FRAME = float(os.environ.get('PACED_FRAME', '0.1'))
WINDOW = 0.5
ARCS = ('click', 'doubleclick', 'drive')


def pause(line):
  word = line.split(maxsplit=1)[0] if line.split() else ''
  return max(FRAME, WINDOW) if word in ARCS else FRAME


def main():
  path, lines = sys.argv[1], sys.argv[2:]
  link = driver.connect(path)
  for line in lines:
    link.settimeout(pause(line))
    link.sendall((line + '\n').encode())
    try:
      while chunk := link.recv(65536):
        sys.stdout.write(chunk.decode())
    except OSError:
      pass
    sys.stdout.flush()
  link.close()


if __name__ == '__main__':
  main()
