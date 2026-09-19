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

WELL_FORMED = """\
# the menu screen
style heading color=#ffd87000 h=12 y=9

panel menu x=8 y=8 w=120 h=64 color=#10203040 anchor=center border=2 edge=#20304050
  label title style=heading text="Set sail" y=4 visible=false wrap=12
  bar health fraction=0.25 z=1.5 frame=0,0.5,0.5,0.5
    image icon frame=icons:2,3
button apply text=OK hover=#405060f0 press=#10141880
row deck pad=6
  column tray pad=2
"""


def baked(tmp_path, text):
  path = tmp_path / "scratch.gui"
  path.write_text(text)
  done = subprocess.run(
    [FIXTURE, "baked", str(path)], cwd=ROOT, capture_output=True, text=True
  )
  return done, str(path)


def test_byte_route_equals_file_route(tmp_path):
  done, _ = baked(tmp_path, WELL_FORMED)
  assert done.returncode == 0, done.stdout
  assert "gui baked tree matches the file's: PASS" in done.stdout
  assert "FAIL" not in done.stdout


def test_collisions_refuse_across_routes(tmp_path):
  done, _ = baked(tmp_path, WELL_FORMED)
  assert "gui baked name collision refuses: PASS" in done.stdout
  assert "gui path collision refuses: PASS" in done.stdout
  assert "already loaded" in done.stdout


def test_malformed_bytes_refuse_on_the_authored_line(tmp_path):
  text = "# a comment\n\npanel menu\n  row deck pad=deep\n"
  done, _ = baked(tmp_path, text)
  assert done.returncode != 0
  assert "baked:4" in done.stdout
  assert "pad=deep" in done.stdout
