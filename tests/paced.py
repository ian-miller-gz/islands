import os
import sys

import driver
import hand

FRAME = float(os.environ.get('PACED_FRAME', '0.1'))
WINDOW = 0.5
ARCS = ('click', 'doubleclick', 'drive')
HOLD = 'hold'
CAP = 120


def pause(line):
  word = line.split(maxsplit=1)[0] if line.split() else ''
  return max(FRAME, WINDOW) if word in ARCS else FRAME


def held(link, line):
  _, marks, command = line.split(maxsplit=2)
  link.settimeout(FRAME)
  answer = ''
  for _ in range(CAP):
    link.sendall((command + '\n').encode())
    answer = answered(link)
    if any(mark in answer for mark in marks.split('|')):
      break
  sys.stdout.write(answer)
  sys.stdout.flush()


def answered(link):
  answer = ''
  for _ in range(CAP):
    try:
      answer = link.recv(65536).decode()
      break
    except OSError:
      continue
  try:
    while answer and (chunk := link.recv(65536)):
      answer += chunk.decode()
  except OSError:
    pass
  return answer


def main():
  path, lines = sys.argv[1], sys.argv[2:]
  link = driver.connect(path)
  words = {HOLD: held, hand.REACH: hand.reach}
  for line in lines:
    if told := words.get(line.partition(' ')[0]):
      told(link, line)
      continue
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
