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

from env import ROOT_DIRECTORY
from make.initializer import Initializer, names

SOURCES = ("src", "cartridges", "hosts")

OVERCLAIMED = ("cartridges/.core/launcher/monitor.poll.cpp",)


def test_a_fragment_names_whole_components() -> None:
  assert names("network/web/server", "/r/src/network/web/server")
  assert names("network/web/server", "/r/src/network/web/server/bind.cpp")
  assert not names("network/web/server", "/r/src/network/web/server.cpp")
  assert not names("network/web/server", "/r/src/network/web/server.policy.cpp")
  assert names("metrics/registry/memory.posix.cpp",
               "/r/src/metrics/registry/memory.posix.cpp")


def test_a_fragment_matches_no_prefix_of_a_component() -> None:
  assert names("/glfw", "/r/submodules/glfw/include")
  assert not names("/glfw", "/r/libs/debug/glfw3")
  assert not names("test/corner", "/r/cartridges/.test/corners/corners.cpp")


def test_the_trailing_slash_spelling_names_the_same_components() -> None:
  for spelling in ("network/web/server", "network/web/server/"):
    assert names(spelling, "/r/src/network/web/server/server.bind.cpp")
    assert not names(spelling, "/r/src/network/web/server.cpp")


def _text(path: str, text: str) -> bool:
  return path in text


def _gate(config: dict, tokens: dict, match, text: str) -> bool:
  for variant in config.get("variants") or []:
    selected = tokens.get(variant.get("selector"))
    for subtree, value in (variant.get("options") or {}).items():
      if match(subtree, text):
        values = value if isinstance(value, list) else [value]
        chosen = selected if isinstance(selected, list) else [selected]
        return any(one in values for one in chosen)
  return True


def _cell(name: str) -> tuple[dict, dict]:
  init = Initializer.__new__(Initializer)
  init.root = ROOT_DIRECTORY
  init.configuration = name
  init.errored = False
  config = init._resolve()
  return config, init._tokens(config)


def _sources() -> list[str]:
  found = []
  for subtree in SOURCES:
    for directory, subdirectories, files in os.walk(ROOT_DIRECTORY / subtree):
      subdirectories[:] = [name for name in subdirectories if name != ".git"]
      found += [os.path.join(directory, file) for file in files
                if file.endswith(".cpp")]
  return found
