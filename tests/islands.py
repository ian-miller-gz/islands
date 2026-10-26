import os

import re

import autest.testers as testers

NAMES = {'SR_VULKAN': 'vulkan', 'SR_SDL3': 'sdl'}
SKINS = {'SR_NATIVE': '', 'SR_RMLUI': 'rmlui'}
FRAMES = 3
WIDTH, HEIGHT = 640, 480
TOLERANCE = 8


def root(directory):
  return os.path.abspath(os.path.join(directory, os.pardir))


def backend(directory):
  header = os.path.join(
    root(directory), 'build', 'include', 'generated', 'preprocessor.hpp')
  with open(header) as file:
    for line in file:
      if line.startswith('#define SR_GFX_BACKEND'):
        return NAMES[line.split()[-1]]
  raise RuntimeError(f'SR_GFX_BACKEND not found in {header}')


def skin(directory, config):
  return os.path.join(config, SKINS[token(directory, 'SR_GUI_BACKEND',
                                          'SR_NATIVE')])


def token(directory, name, default='SR_NONE'):
  header = os.path.join(
    root(directory), 'build', 'include', 'generated', 'preprocessor.hpp')
  with open(header) as file:
    for line in file:
      if line.startswith(f'#define {name} '):
        return line.split()[-1]
  return default


def cartridge(directory):
  return token(directory, 'SR_CARTRIDGE')


def bundle(directory):
  return token(directory, 'SR_CARTRIDGE_BUNDLE', '').strip('"')


def listener(directory):
  return token(directory, 'SR_LISTEN')


def listening(test, condition):
  test.SkipIf(condition(
    lambda: listener(test.TestDirectory) == 'SR_NONE',
    'the binary compiles no terminal listener (SR_LISTEN: SR_NONE)'))


def measured(test, condition):
  test.SkipIf(condition(
    lambda: token(test.TestDirectory, 'SR_METRICS', 'SR_REGISTRY')
    == 'SR_NONE',
    'the binary compiles no metrics registry (SR_METRICS: SR_NONE)'))


def dynamic(test, condition):
  test.SkipIf(condition(
    lambda: cartridge(test.TestDirectory) != 'SR_NONE',
    'the binary links its cartridge statically (SR_CARTRIDGE)'))


def static(test, condition):
  test.SkipIf(condition(
    lambda: cartridge(test.TestDirectory) == 'SR_NONE',
    'the binary runs the dynamic loader (SR_CARTRIDGE: SR_NONE)'))


def debug(test, condition):
  test.SkipIf(condition(
    lambda: token(test.TestDirectory, 'SR_PROFILE', 'SR_DEBUG') != 'SR_DEBUG',
    'the binary compiles the release profile (SR_PROFILE: SR_RELEASE)'))


def native(test, condition):
  test.SkipIf(condition(
    lambda: token(test.TestDirectory, 'SR_GUI_BACKEND', 'SR_NATIVE')
    != 'SR_NATIVE',
    'the binary compiles a non-native GUI backend (SR_GUI_BACKEND)'))


def release(test, condition):
  test.SkipIf(condition(
    lambda: token(test.TestDirectory, 'SR_PROFILE', 'SR_DEBUG') == 'SR_DEBUG',
    'the binary compiles the debug profile (SR_PROFILE: SR_DEBUG)'))


CARRIED = ('SR_GFX_BACKEND', 'SR_WINDOW_BACKEND', 'SR_CARTRIDGE', 'SR_PROFILE')


def bench(directory):
  import yaml
  with open(os.path.join(root(directory), 'configs', 'make.yaml')) as file:
    groups = (yaml.safe_load(file) or {}).get('tokens') or {}
  flat = {}
  for name, value in groups.items():
    flat.update(value if isinstance(value, dict) else {name: value})
  return {name: value[0] if isinstance(value, list) else value
          for name, value in flat.items()
          if isinstance(value, (str, list)) and name not in CARRIED}


def benched(test, condition):
  for name, expected in bench(test.TestDirectory).items():
    test.SkipIf(condition(
      lambda name=name, expected=expected:
        token(test.TestDirectory, name, expected) != expected,
      f'the binary compiles a non-bench {name} (the gold pins {expected})'))


def carried(directory):
  header = os.path.join(
    root(directory), 'build', 'include', 'generated', 'preprocessor.hpp')
  with open(header) as file:
    for line in file:
      if line.startswith('#define SR_NETWORK_BACKEND_SET '):
        return re.findall(r'SR_\w+', line)[1:]
  return [token(directory, 'SR_NETWORK_BACKEND', 'SR_UNIX')]


def carries(directory, value):
  return value in carried(directory)


SCHEMES = (('SR_UNIX', 'unix'), ('SR_ABSTRACT', 'abstract'),
           ('SR_TCP', 'tcp'), ('SR_TCP6', 'tcp6'), ('SR_SCTP', 'sctp'),
           ('SR_VSOCK', 'vsock'), ('SR_TLS', 'tls'))

SPOKEN = ('unix', 'tcp', 'tcp6', 'tls')


def stream(directory):
  members = carried(directory)
  return next(word for name, word in SCHEMES if name in members)


def transported(test, condition):
  test.SkipIf(condition(
    lambda: stream(test.TestDirectory) not in SPOKEN,
    'the gold drivers do not speak the composed stream transport'))


def binary(directory):
  return os.path.join(root(directory), 'build', 'Island')


def flags(**extra):
  values = {
    'frames': FRAMES, 'width': WIDTH, 'height': HEIGHT, 'overlay': 0,
    'visible': 0, 'window': 'console=0', 'midi': 'none'}
  values.update(extra)
  return ' '.join(f'--{key} {value}' for key, value in values.items())


def gold(directory, config, name):
  return os.path.join(directory, 'gold', config, name)


def counted(test, pattern, times, transcript='paced.out'):
  expression = re.compile(pattern, re.M)

  def counting(data):
    found = len(expression.findall(data))
    return (None if found == times else
            f'{found} lines match {pattern}, not {times}')

  test.Disk.File(transcript).Content += testers.FileContentCallback(
    counting, f'{transcript} holds {times} lines matching {pattern}')
