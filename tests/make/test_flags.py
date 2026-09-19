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
from pathlib import Path as PyPath
from types import SimpleNamespace

from env import Path as LocalPath
from make.compiler import Compiler
from make.initializer import Initializer
from make.precompiler import Precompiler


def test_flags_selects_matching_option() -> None:
  init = Initializer.__new__(Initializer)
  init._project_cfg = {
    "tokens": {"SR_PROFILE": "SR_RELEASE"},
    "flags": [
      {
        "selector": "SR_PROFILE",
        "options": {"SR_DEBUG": [], "SR_RELEASE": ["-O2", "-DNDEBUG"]},
      }
    ],
  }
  assert init._flags() == ["-O2", "-DNDEBUG"]


def test_flags_empty_option_selects_nothing() -> None:
  init = Initializer.__new__(Initializer)
  init._project_cfg = {
    "tokens": {"SR_PROFILE": "SR_DEBUG"},
    "flags": [
      {
        "selector": "SR_PROFILE",
        "options": {"SR_DEBUG": [], "SR_RELEASE": ["-O2", "-DNDEBUG"]},
      }
    ],
  }
  assert init._flags() == []


def test_flags_empty_without_section() -> None:
  init = Initializer.__new__(Initializer)
  init._project_cfg = {"tokens": {"SR_PROFILE": "SR_DEBUG"}}
  assert init._flags() == []


def test_flags_unknown_selection_selects_nothing() -> None:
  init = Initializer.__new__(Initializer)
  init._project_cfg = {
    "flags": [{"selector": "SR_PROFILE", "options": {"SR_RELEASE": ["-O2"]}}]
  }
  assert init._flags() == []


def test_compile_command_carries_flags() -> None:
  make: SimpleNamespace = SimpleNamespace()
  make.flags = ["-O2", "-DNDEBUG"]
  make.libraries = []
  make.includes = set()
  make.roots = []
  make.include_paths = set()
  compiler = Compiler(make)
  command = compiler._build_command(
    LocalPath("src/island.cpp"), LocalPath("build/src/wip.island.o")
  )
  assert "-O2" in command and "-DNDEBUG" in command


def test_precompile_command_carries_flags() -> None:
  make = SimpleNamespace(
    root=LocalPath("/project"), flags=["-O2"], roots=[],
    include_paths=set())
  precompiler = Precompiler(make)
  command = precompiler._command(
    LocalPath("src/common.hpp"),
    LocalPath("build/objects/zone/precompiled/pic/src/common.hpp.gch"),
    "pic",
  )
  assert "-O2" in command and "-fPIC" in command and "-MD" in command


def test_targets_render_their_own_defines() -> None:
  init = Initializer.__new__(Initializer)
  init._project_cfg = {
    "libraries": [
      {
        "name": "remote",
        "path": "cartridges/.test/remote",
        "tokens": {"SR_WIRE": "SR_UNIX"},
        "directives": {"BANNER": "Remote", "LIMIT": 3, "TRACING": True, "OFF": False},
      }
    ],
    "builds": [
      {"name": "Islands", "path": "src/island", "tokens": {"SR_HOST": 1}},
      {"name": "probe", "path": "src/test/probe"},
    ],
  }
  assert init._libraries()[0]["defines"] == [
    "-DSR_WIRE=SR_UNIX",
    '-DBANNER="Remote"',
    "-DLIMIT=3",
    "-DTRACING=1",
  ]
  assert init._builds()[0]["defines"] == ["-DSR_HOST=1"]
  assert init._builds()[1]["defines"] == []


def test_defines_reach_only_the_declaring_subtree() -> None:
  make: SimpleNamespace = SimpleNamespace()
  make.libraries = [
    {
      "name": "remote",
      "path": PyPath("cartridges/.test/remote"),
      "defines": ["-DSR_WIRE=SR_UNIX"],
    }
  ]
  make.builds = []
  make.includes = set()
  make.roots = []
  make.include_paths = set()
  compiler = Compiler(make)
  assert compiler._flags(PyPath("cartridges/.test/remote/sessions.cpp")) == [
    "-fPIC",
    "-fno-gnu-unique",
    "-DSR_WIRE=SR_UNIX",
  ]
  assert compiler._flags(PyPath("src/island/island.cpp")) == []
  command = compiler._build_command(
    LocalPath("cartridges/.test/remote/sessions.cpp"),
    LocalPath("build/objects/cartridges/.test/remote/wip.sessions.o"),
    compiler._flags(PyPath("cartridges/.test/remote/sessions.cpp")),
  )
  assert "-DSR_WIRE=SR_UNIX" in command


def test_build_entry_defines_reach_its_subtree() -> None:
  make: SimpleNamespace = SimpleNamespace()
  make.libraries = []
  make.builds = [
    {"name": "Islandsd", "path": PyPath("src/reef"), "defines": ["-DSR_HOST=1"]}
  ]
  compiler = Compiler(make)
  assert compiler._flags(PyPath("src/reef/reef.cpp")) == ["-DSR_HOST=1"]
  assert compiler._flags(PyPath("src/island/island.cpp")) == []


def test_statics_carry_target_defines() -> None:
  make: SimpleNamespace = SimpleNamespace()
  make.libraries = [
    {
      "name": "meadow",
      "path": PyPath("cartridges/.test/meadow"),
      "static": True,
      "defines": ["-DSR_EMBED=1"],
    }
  ]
  make.builds = []
  compiler = Compiler(make)
  assert compiler._flags(PyPath("cartridges/.test/meadow/cartridge.cpp")) == ["-DSR_EMBED=1"]


def test_longest_prefix_target_wins() -> None:
  make: SimpleNamespace = SimpleNamespace()
  make.libraries = [
    {"name": "outer", "path": PyPath("cartridges/.test"), "defines": ["-DOUTER=1"]},
    {"name": "inner", "path": PyPath("cartridges/.test/remote"), "defines": ["-DINNER=1"]},
  ]
  make.builds = []
  compiler = Compiler(make)
  assert compiler._defines(PyPath("cartridges/.test/remote/cartridge.cpp")) == ["-DINNER=1"]
  assert compiler._defines(PyPath("cartridges/.test/viewer/cartridge.cpp")) == ["-DOUTER=1"]
