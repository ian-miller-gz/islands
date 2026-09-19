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


def parse(tmp_path, text):
  path = tmp_path / "scratch.gui"
  path.write_text(text)
  done = subprocess.run([FIXTURE, str(path)], cwd=ROOT, capture_output=True, text=True)
  return done, str(path)


def nodes(stdout):
  lines = [line for line in stdout.splitlines() if " kind=" in line]
  return {line.split()[0]: line for line in lines}


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


def test_round_trip(tmp_path):
  done, _ = parse(tmp_path, WELL_FORMED)
  assert done.returncode == 0, done.stdout
  tree = nodes(done.stdout)
  assert "x=8 y=8 w=120 h=64" in tree["menu"]
  assert "anchor=center color=#10203040" in tree["menu"]
  assert "border=2 edge=#20304050" in tree["menu"]
  assert "fraction=0.25" in tree["health"]
  assert "z=1.5" in tree["health"]
  assert "frame=0,0.5,0.5,0.5" in tree["health"]
  assert 'text="Set sail"' in tree["title"]
  assert "visible=false" in tree["title"]
  assert "wrap=12" in tree["title"]
  assert 'text="OK"' in tree["apply"]
  assert "hover=#405060f0" in tree["apply"]
  assert "press=#10141880" in tree["apply"]
  assert "pad=6" in tree["deck"]
  assert "pad=2" in tree["tray"]


def test_named_frames_resolve_through_the_binding(tmp_path):
  done, _ = parse(tmp_path, WELL_FORMED)
  tree = nodes(done.stdout)
  assert "frame=2,3,1,1" in tree["icon"]


def test_indentation_is_the_tree_edge(tmp_path):
  done, _ = parse(tmp_path, WELL_FORMED)
  tree = nodes(done.stdout)
  assert "kind=panel parent= " in tree["menu"]
  assert "kind=label parent=menu" in tree["title"]
  assert "kind=image parent=health" in tree["icon"]
  assert "kind=button parent= " in tree["apply"]
  assert "kind=column parent=deck" in tree["tray"]


def test_styles_expand_and_inline_wins(tmp_path):
  done, _ = parse(tmp_path, WELL_FORMED)
  tree = nodes(done.stdout)
  assert "color=#ffd87000" in tree["title"]
  assert "h=12" in tree["title"]
  assert "y=4" in tree["title"]


def test_untouched_keys_hold_their_defaults(tmp_path):
  done, _ = parse(tmp_path, WELL_FORMED)
  tree = nodes(done.stdout)
  assert "anchor=northwest" in tree["title"]
  assert "color=#283038f0" in tree["apply"]
  assert "border=0 edge=#8898a8ff" in tree["apply"]
  assert "color=#c8d0dcff" in tree["health"]
  assert "edge=#242c34ff" in tree["health"]
  assert "pad=0" in tree["menu"]
  assert "fraction=0 z=0" in tree["menu"]
  assert 'text="" frame=0,0,1,1 visible=true' in tree["menu"]
  assert "wrap=0" in tree["menu"]
  assert "border=0" in tree["title"]


INHERITED = """\
style base color=#11223344 h=12 border=2 wrap=8
style loud style=base color=#ff0000ff border=5
panel plain style=base w=40
label shout style=loud text=Hi w=40
label mixed style=loud color=#00ff00ff w=40
"""


def test_style_inheritance_copies_then_overrides(tmp_path):
  done, _ = parse(tmp_path, INHERITED)
  assert done.returncode == 0, done.stdout
  tree = nodes(done.stdout)
  assert "color=#11223344" in tree["plain"]
  assert "border=2" in tree["plain"]
  assert "color=#ff0000ff" in tree["shout"]
  assert "border=5" in tree["shout"]
  assert "h=12" in tree["shout"]
  assert "wrap=8" in tree["shout"]


def test_inline_still_wins_over_an_inherited_style(tmp_path):
  done, _ = parse(tmp_path, INHERITED)
  tree = nodes(done.stdout)
  assert "color=#00ff00ff" in tree["mixed"]
  assert "border=5" in tree["mixed"]


SHEET = """\
# a scratch theme
style ink color=#11223344
style plate style=ink border=2
"""


def sheet(tmp_path, text=SHEET):
  path = tmp_path / "theme.gui"
  path.write_text(text)
  return str(path)


def test_an_included_sheet_defines_styles(tmp_path):
  done, _ = parse(tmp_path, f"style={sheet(tmp_path)}\nlabel a style=ink w=4\n")
  assert done.returncode == 0, done.stdout
  assert "color=#11223344" in nodes(done.stdout)["a"]


def test_a_sheet_style_serves_as_a_parent(tmp_path):
  text = (
    f"style={sheet(tmp_path)}\n"
    "style loud style=plate color=#ff0000ff\n"
    "label a style=loud\n"
  )
  done, _ = parse(tmp_path, text)
  assert done.returncode == 0, done.stdout
  assert "color=#ff0000ff" in nodes(done.stdout)["a"]
  assert "border=2" in nodes(done.stdout)["a"]


def test_a_later_definition_overrides_the_sheet(tmp_path):
  text = f"style={sheet(tmp_path)}\nstyle ink color=#ff0000ff\nlabel a style=ink\n"
  done, _ = parse(tmp_path, text)
  assert done.returncode == 0, done.stdout
  assert "color=#ff0000ff" in nodes(done.stdout)["a"]


def test_the_engine_sheet_dresses_the_documented_names(tmp_path):
  names = ["ink", "muted", "sheet", "box", "place", "entry", "call", "selected", "paper"]
  text = "style=assets/islands.gui\n" + "".join(
    f"label u{at} style={name}\n" for at, name in enumerate(names)
  )
  done, _ = parse(tmp_path, text)
  assert done.returncode == 0, done.stdout


SHEET_ONLY = [
  ("panel a w=4", "a widget line"),
  ("style=another.gui", "a nested include"),
]


@pytest.mark.parametrize("line,name", SHEET_ONLY, ids=[name for _, name in SHEET_ONLY])
def test_a_sheet_holds_style_definitions_only(tmp_path, line, name):
  bad = sheet(tmp_path, SHEET + line + "\n")
  done, _ = parse(tmp_path, f"style={bad}\nlabel a w=4\n")
  assert done.returncode != 0
  assert f"{bad}:4" in done.stdout
  assert "a sheet holds style definitions only" in done.stdout
  assert not nodes(done.stdout), "a refused document must print no nodes"


def test_a_missing_sheet_refuses(tmp_path):
  done, path = parse(tmp_path, f"style={tmp_path}/absent.gui\nlabel a w=4\n")
  assert done.returncode != 0
  assert f"{path}:1" in done.stdout
  assert "cannot read the sheet" in done.stdout
  assert not nodes(done.stdout)


def test_the_include_is_top_level_and_alone(tmp_path):
  done, path = parse(tmp_path, f"panel a w=4\n  style={sheet(tmp_path)}\n")
  assert done.returncode != 0
  assert f"{path}:2" in done.stdout
  assert "the include is top-level" in done.stdout
  done, path = parse(tmp_path, f"style={sheet(tmp_path)} extra=1\n")
  assert done.returncode != 0
  assert f"{path}:1" in done.stdout
  assert "the include stands alone on its line" in done.stdout


REFUSALS = [
  ("wizard w x=4", 1, "unknown kind 'wizard'"),
  ("style child style=missing", 1, "undefined style 'missing'"),
  ("panel a\npanel a", 2, "node refused 'a'"),
  ("panel a\n    panel b", 2, "indent jumps more than one level"),
  ("\tpanel a", 1, "indentation must be spaces"),
  ("panel a\n panel b", 2, "indentation must be spaces"),
  ('label a text="oops', 1, "unterminated quote"),
  ("panel a style=ghost", 1, "undefined style 'ghost'"),
  ("panel a\n  style heading x=1", 2, "style definitions are top-level"),
  ("panel a colr=#11223344", 1, "refused property 'colr=#11223344'"),
  ("panel a x=wide", 1, "refused property 'x=wide'"),
  ("panel a color=#1122", 1, "refused property 'color=#1122'"),
  ("panel a visible=maybe", 1, "refused property 'visible=maybe'"),
  ("panel a frame=0,0,1", 1, "refused property 'frame=0,0,1'"),
  ("label a wrap=-2", 1, "refused property 'wrap=-2'"),
  ("panel a border=thin", 1, "refused property 'border=thin'"),
  ("row a pad=deep", 1, "refused property 'pad=deep'"),
  ("button a hover=#1122", 1, "refused property 'hover=#1122'"),
  ("button a glow=#1122", 1, "refused property 'glow=#1122'"),
  ("image a frame=ghost:1,2", 1, "refused property 'frame=ghost:1,2'"),
  ("image a frame=icons:1", 1, "refused property 'frame=icons:1'"),
]


@pytest.mark.parametrize(
  "text,line,token", REFUSALS, ids=[token for _, _, token in REFUSALS]
)
def test_malformed_documents_refuse_whole(tmp_path, text, line, token):
  done, path = parse(tmp_path, text)
  assert done.returncode != 0
  assert f"{path}:{line}" in done.stdout
  assert token in done.stdout
  assert not nodes(done.stdout), "a refused document must print no nodes"


def test_missing_file_refuses(tmp_path):
  done = subprocess.run(
    [FIXTURE, str(tmp_path / "absent.gui")], cwd=ROOT, capture_output=True, text=True
  )
  assert done.returncode != 0
  assert "Cannot read the document" in done.stdout


SEATED = """\
panel page w=640 h=480
  button door x=200 y=100 w=120 h=40
  panel plate w=160 h=80 beside=door side=below air=6
  panel pane w=90 h=60 beside=door side=east
"""


def test_the_seat_keys_round_trip(tmp_path):
  done, _ = parse(tmp_path, SEATED)
  assert done.returncode == 0, done.stdout
  tree = nodes(done.stdout)
  assert "beside=door side=below air=6" in tree["plate"]
  assert "beside=door side=east air=0" in tree["pane"]
  assert "beside=" not in tree["door"]


@pytest.mark.parametrize(
  "line, token",
  [
    ("  panel plate beside=plate side=below", "beside=plate"),
    ("  panel plate beside=ghost side=below", "beside=ghost"),
    ("  panel plate beside=door side=northwest", "side=northwest"),
    ("  panel plate beside=door air=wide", "air=wide"),
  ],
)
def test_a_refused_seat_discards_the_document(tmp_path, line, token):
  text = "panel page w=640 h=480\n  button door x=10 y=10 w=20 h=20\n" + line + "\n"
  done, path = parse(tmp_path, text)
  assert done.returncode != 0
  assert token in done.stdout
  assert not nodes(done.stdout)


CHAIN = """\
panel page w=640 h=480
  panel one w=20 h=20{ring}
  panel two w=20 h=20 beside=one side=below
  panel three w=20 h=20 beside=two side=below
"""


def test_a_seat_chain_is_fine(tmp_path):
  assert parse(tmp_path, CHAIN.format(ring=""))[0].returncode == 0


def test_a_seat_ring_refuses(tmp_path):
  done, _ = parse(tmp_path, CHAIN.format(ring=" beside=three side=below"))
  assert done.returncode != 0
  assert "beside=three" in done.stdout
  assert not nodes(done.stdout)
