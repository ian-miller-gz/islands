import os
import subprocess

import pytest

ROOT = os.path.abspath(
  os.path.join(os.path.dirname(__file__), *[os.pardir] * 2))
ISLAND = os.path.join(ROOT, 'build', 'Island')
LAUNCHER = 'cartridges/.core/launcher'
QUIET = ('--visible', '0', '--overlay', '0', '--window', 'console=0',
         '--midi', 'none')

pytestmark = pytest.mark.skipif(
  not os.access(ISLAND, os.X_OK), reason='no build/Island')


def run(runtime, *flags):
  env = {**os.environ, 'SDL_AUDIO_DRIVER': 'absent',
         'XDG_RUNTIME_DIR': str(runtime)}
  return subprocess.run(
    [ISLAND, *flags], cwd=ROOT, env=env, capture_output=True, text=True,
    timeout=120)


def bundle(tmp_path, host):
  seat = tmp_path / 'shore'
  seat.mkdir()
  (seat / 'manifest.yaml').write_text(
    f'name: Shore\nhost: {host}\nentry: libnothing.so\n')
  return str(seat)


def test_a_reef_bundle_runs_on_reef(tmp_path):
  done = run(tmp_path, '--cartridge', bundle(tmp_path, 'reef'))
  assert '~/reef::' in done.stdout
  assert '~/island::' not in done.stdout


def test_no_bundle_runs_the_launcher(tmp_path):
  done = run(tmp_path, '--frames', '1', *QUIET)
  assert f'Loading {LAUNCHER}...' in done.stdout
  assert done.returncode == 0


def test_none_runs_bare(tmp_path):
  done = run(tmp_path, '--cartridge', 'none', '--frames', '1', *QUIET)
  assert 'Running without a cartridge.' in done.stdout
  assert done.returncode == 0


def test_an_unknown_host_is_refused(tmp_path):
  done = run(tmp_path, '--cartridge', bundle(tmp_path, 'shore'))
  assert done.returncode == 2
  assert 'Unknown host' in done.stdout
  assert '~/island::' not in done.stdout
