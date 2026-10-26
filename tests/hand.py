import math
import os
import re
import shlex
import sys

from paced import FRAME, WINDOW

REACH, UNREACHED = 'reach', 'unreached'
PATH, ROW, CELL = '>', '#', '.'
BARE = ('key', 'type')
NOTCH, CAP, DOWN, HALF = 3, 1000, -1, 0.5
CHUNK = 65536
PACES = {'fast': 1, 'normal': 3, 'cinematic': 10}
PACE, FAST = 'PACE', 'fast'
READ = re.compile(r' rect=([-\d.]+),([-\d.]+) ([\d.]+)x([\d.]+)')
REFUSED = re.compile(r'^\w+ refused: ', re.M)


def sent(link, line, budget):
  link.settimeout(budget * PACES[os.environ.get(PACE, FAST)])
  link.sendall((line + '\n').encode())
  answer = ''
  try:
    while chunk := link.recv(CHUNK):
      answer += chunk.decode()
  except OSError:
    pass
  sys.stdout.write(answer)
  sys.stdout.flush()
  return answer


def window(*answers):
  box, zero, one = ([float(value) for value in READ.search(answer).groups()]
                    for answer in answers)
  step = one[1] - zero[1]
  return (box[3] + step - zero[3]) // step if step > 0 else math.inf


def turns(row, shown):
  rows = max(0, row - shown + 1)
  return DOWN * (rows + HALF) / NOTCH if rows else 0


def first(spun):
  return max(0, math.floor(-spun * NOTCH))


def seat(link, target):
  if ROW not in target:
    return target
  name, rest = target.split(ROW, 1)
  row, _, cell = rest.partition(CELL)
  ids = (name, f'{name}{CELL}0', f'{name}{CELL}1')
  read = [sent(link, f'read {each}', FRAME) for each in ids]
  spun = turns(int(row), window(*read))
  for turn in (-DOWN * CAP, spun):
    if turn and REFUSED.search(sent(link, f'wheel {ids[1]} {turn:g}', FRAME)):
      return None
  return CELL.join(filter(None, [name, str(int(row) - first(spun)), cell]))


def hovered(link, target):
  spot = seat(link, target)
  if spot and not REFUSED.search(sent(link, f'hover {spot}', FRAME)):
    return spot
  print(f'{UNREACHED} {spot or target}', flush=True)


def reach(link, line):
  _, path, word, *args = line.split()
  *way, last = path.split(PATH)
  arc = max(FRAME, WINDOW)
  for target in way:
    if (spot := hovered(link, target)) is None:
      return
    sent(link, f'click {spot}', arc)
  if (spot := hovered(link, last)) is not None:
    sent(link, ' '.join([word, *[spot] * (word not in BARE), *args]), arc)


def words(command):
  return [leg.split()[0] for leg in shlex.split(command) if leg.split()]
