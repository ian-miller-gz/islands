#!/usr/bin/env python3
import sys


def load(path):
  with open(path, 'rb') as file:
    data = file.read()
  fields = []
  offset = 0
  while len(fields) < 4:
    while data[offset] in b' \t\r\n':
      offset += 1
    start = offset
    while data[offset] not in b' \t\r\n':
      offset += 1
    fields.append(data[start:offset])
  offset += 1
  magic, width, height, depth = fields
  if magic != b'P6':
    raise ValueError(f'{path} is not a binary PPM (P6)')
  return int(width), int(height), data[offset:]


def main():
  gold, actual, tolerance = sys.argv[1], sys.argv[2], int(sys.argv[3])
  shapes, pixels = zip(*((size[:2], size[2]) for size in map(load, (gold, actual))))
  if shapes[0] != shapes[1] or len(pixels[0]) != len(pixels[1]):
    print(f'shape mismatch: gold {shapes[0]} vs actual {shapes[1]}')
    return 2
  worst = max(abs(a - b) for a, b in zip(pixels[0], pixels[1]))
  print(f'worst channel difference: {worst} (tolerance {tolerance})')
  return 0 if worst <= tolerance else 1


if __name__ == '__main__':
  sys.exit(main())
