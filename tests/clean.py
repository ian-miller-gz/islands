import os
import subprocess
import sys

PTY = 'forkpty'
SH = '/bin/sh'
KIT = '6TOPICS7DURABLE'


def symbols(path):
  out = subprocess.run(['nm', '-C', path], capture_output=True, text=True)
  return out.stdout


def texts(path):
  out = subprocess.run(['strings', path], capture_output=True, text=True)
  return out.stdout.splitlines()


def carries(path):
  return PTY in symbols(path) or SH in texts(path)


def check(verdict, what):
  print(f'  {"PASS" if verdict else "FAIL"}: {what}')
  return 0 if verdict else 1


def shell(root):
  status = 0
  for host in ('Island', 'Reef'):
    path = os.path.join(root, 'build', host)
    if not os.path.exists(path):
      return check(False, f'{host} is not built')
    status |= check(not carries(path), f'{host} carries no OS shell')
  for name in ('libshell.a', 'libshell.so'):
    path = staged(root, name)
    if not path:
      return check(False, f'{name} is not built')
    status |= check(carries(path), f'{name} carries the OS shell')
  return status


def durable(root):
  status = 0
  for host in ('Island', 'Reef'):
    path = os.path.join(root, 'build', host)
    if not os.path.exists(path):
      return check(False, f'{host} is not built')
    status |= check(
      KIT not in symbols(path) and not any(KIT in line for line in texts(path)),
      f'{host} carries no durable schedule tier')
  path = staged(root, 'libdurable.a')
  if not path:
    return check(False, 'libdurable.a is not built')
  return status | check(
    any(KIT in line for line in texts(path)),
    'libdurable.a carries the durable schedule tier')


def staged(root, name):
  for directory, _, files in os.walk(os.path.join(root, 'build', 'outputs')):
    if name in files:
      return os.path.join(directory, name)
  return None


if __name__ == '__main__':
  CHECKS = {'shell': shell, 'durable': durable}
  if len(sys.argv) != 3 or sys.argv[1] not in CHECKS:
    print('usage: clean.py {shell|durable} <root>')
    sys.exit(2)
  sys.exit(CHECKS[sys.argv[1]](sys.argv[2]))
