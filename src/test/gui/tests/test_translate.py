__copyright__ = """
 ===========================================================================
 *              (C) Copyright 2025 - Islands-Engine - Ian Miller             *
 =============================================================================
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Affero General Public License as           *
 *  published by the Free Software Foundation, either version 3 of the       *
 *  License, or (at your option) any later version.                          *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Affero General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Affero General Public License *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ============================================================================"""
import os
import subprocess

import pytest


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), *[os.pardir] * 4))
FIXTURE = os.path.join(ROOT, "build", "outputs", "gui.out")

pytestmark = pytest.mark.skipif(
  not os.path.exists(FIXTURE),
  reason="gui fixture not built (python3 -m make)",
)


def emit(text, tmp_path, name="scratch.gui"):
  path = tmp_path / name
  path.write_text(text)
  return subprocess.run(
    [FIXTURE, "emit", str(path)], cwd=ROOT, capture_output=True, text=True
  )


def dump(text, tmp_path, name="scratch.gui"):
  path = tmp_path / name
  path.write_text(text)
  return subprocess.run(
    [FIXTURE, str(path)], cwd=ROOT, capture_output=True, text=True
  )


def tree(stdout):
  lines = [line for line in stdout.splitlines() if " kind=" in line]
  return {line.split()[0]: line for line in lines}


def joins(stdout):
  return [line for line in stdout.splitlines() if line.startswith("link ")]


ROUND = """\
style heading color=#ffd870ff h=12
panel menu x=8 y=8 w=120 h=64 color=#10203040 anchor=center border=2 edge=#20304050
  label title style=heading text="Set sail" y=4 visible=false wrap=12
  bar health fraction=0.25 z=1.5 frame=0,0.5,0.5,0.5
  image crest frame=icons:2,3 w=16 h=16
button apply text=OK hover=#405060f0 press=#10141880
row deck pad=6
  column tray pad=2
graph wall x=8 y=200 w=200 h=120 pan=4,2 zoom=1.5
  node osc x=10 y=10 w=60 h=30 port=out,audio text="osc"
  node amp x=100 y=10 w=60 h=30 port=in,audio text="amp"
link osc amp "excites"
"""


def test_emit_reparses_to_an_equal_tree(tmp_path):
  original = dump(ROUND, tmp_path, "original.gui")
  assert original.returncode == 0, original.stdout
  emitted = emit(ROUND, tmp_path)
  assert emitted.returncode == 0, emitted.stdout
  reparsed = dump(emitted.stdout, tmp_path, "emitted.gui")
  assert reparsed.returncode == 0, reparsed.stdout
  assert tree(reparsed.stdout) == tree(original.stdout)


def test_emit_round_trips_the_links(tmp_path):
  original = dump(ROUND, tmp_path, "original.gui")
  emitted = emit(ROUND, tmp_path)
  assert emitted.returncode == 0, emitted.stdout
  assert 'link osc amp "excites"' in emitted.stdout.splitlines()
  reparsed = dump(emitted.stdout, tmp_path, "emitted.gui")
  assert reparsed.returncode == 0, reparsed.stdout
  assert joins(reparsed.stdout) == joins(original.stdout)


def test_emit_of_the_canonical_text_is_byte_identical(tmp_path):
  once = emit(ROUND, tmp_path, "once.gui")
  assert once.returncode == 0, once.stdout
  twice = emit(once.stdout, tmp_path, "twice.gui")
  assert twice.returncode == 0, twice.stdout
  assert twice.stdout == once.stdout


def test_emit_is_deterministic(tmp_path):
  first = emit(ROUND, tmp_path, "a.gui")
  second = emit(ROUND, tmp_path, "b.gui")
  assert first.stdout == second.stdout
  assert first.returncode == 0


def test_emit_is_style_free(tmp_path):
  emitted = emit(ROUND, tmp_path)
  assert "style=" not in emitted.stdout
  assert not any(line.startswith("style ") for line in emitted.stdout.splitlines())


def test_emit_re_expresses_a_bound_atlas(tmp_path):
  bound = "image crest frame=icons:2,3 w=16 h=16\n"
  done = emit(bound, tmp_path)
  assert done.returncode == 0, done.stdout
  assert "frame=icons:2,3" in done.stdout
  reparsed = dump(done.stdout, tmp_path, "emitted.gui")
  original = dump(bound, tmp_path, "original.gui")
  assert reparsed.returncode == 0, reparsed.stdout
  assert tree(reparsed.stdout) == tree(original.stdout)


def test_emit_writes_a_port_kind(tmp_path):
  emitted = emit(ROUND, tmp_path)
  assert emitted.returncode == 0, emitted.stdout
  assert " port=out,audio " in emitted.stdout
  assert " port=in,audio " in emitted.stdout
  assert " port=none " in emitted.stdout
  twice = emit(emitted.stdout, tmp_path, "twice.gui")
  assert twice.returncode == 0, twice.stdout
  assert twice.stdout == emitted.stdout
