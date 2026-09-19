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
import yaml

from make.config import PROJECT_CONFIG_PATH
from make.make import Make
from env import Path


def _configure(tmp_path) -> None:
  config = {
    "tokens": {"values": {"NONE": 1}},
    "libraries": [
      {"name": "thing", "path": "cartridges/thing", "output_destinations": []}],
    "precompiled": [
      {"header": "src/common.hpp", "subtrees": ["src", "cartridges"]}],
  }
  path = tmp_path / PROJECT_CONFIG_PATH
  path.parent.mkdir(parents=True, exist_ok=True)
  with open(path, "w", encoding="utf-8") as file:
    yaml.safe_dump(config, file)


def _sources(tmp_path) -> None:
  source = tmp_path / "src"
  source.mkdir(exist_ok=True)
  (source / "inner.hpp").write_text("#pragma once\ninline int inner() { return 1; }\n")
  (source / "common.hpp").write_text("#pragma once\n#include <inner.hpp>\n")
  (source / "main.cpp").write_text("#include <common.hpp>\nint main() { return inner(); }\n")
  library = tmp_path / "cartridges" / "thing"
  library.mkdir(parents=True, exist_ok=True)
  (library / "thing.cpp").write_text("#include <common.hpp>\nint thing() { return inner(); }\n")


def _build(monkeypatch, tmp_path) -> Make:
  monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
  make = Make()
  make.preprocess()
  make.precompile()
  make.compile()
  make.clear()
  return make


def _seats(tmp_path, make: Make) -> dict[str, Path]:
  zone = tmp_path / "build" / "objects" / make.zone / "precompiled"
  return {flavour: zone / flavour / "src" / "common.hpp"
          for flavour in ("plain", "pic")}


def test_pch_builds_per_flavour_and_reuses_while_current(
  monkeypatch, tmp_path
) -> None:
  _configure(tmp_path)
  _sources(tmp_path)
  make = _build(monkeypatch, tmp_path)
  assert not make.errored
  seats = _seats(tmp_path, make)
  for seat in seats.values():
    assert seat.with_name(seat.name + ".gch").exists()
  records = {
    flavour: seat.with_name(seat.name + ".yaml").read_text()
    for flavour, seat in seats.items()}
  make = _build(monkeypatch, tmp_path)
  assert not make.errored
  for flavour, seat in seats.items():
    assert seat.with_name(seat.name + ".yaml").read_text() == records[flavour]


def test_deep_header_edit_rebuilds_the_pch(monkeypatch, tmp_path) -> None:
  _configure(tmp_path)
  _sources(tmp_path)
  make = _build(monkeypatch, tmp_path)
  seat = _seats(tmp_path, make)["plain"]
  record = seat.with_name(seat.name + ".yaml").read_text()
  (tmp_path / "src" / "inner.hpp").write_text(
    "#pragma once\ninline int inner() { return 2; }\n")
  make = _build(monkeypatch, tmp_path)
  assert not make.errored
  assert seat.with_name(seat.name + ".yaml").read_text() != record


def test_declared_subtrees_consume_their_flavour(monkeypatch, tmp_path) -> None:
  _configure(tmp_path)
  _sources(tmp_path)
  monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
  make = Make()
  plain = make.compiler._prefix(Path("src/main.cpp"), [])
  pic = make.compiler._prefix(
    Path("cartridges/thing/thing.cpp"), ["-fPIC", "-fno-gnu-unique"])
  assert "-include" in plain and "-Winvalid-pch" in plain and "-fpch-deps" in plain
  assert str(_seats(tmp_path, make)["plain"]) in plain
  assert str(_seats(tmp_path, make)["pic"]) in pic
  assert make.compiler._prefix(Path("libs/other/other.cpp"), []) == []


def test_failed_precompile_warns_and_leaves_no_gch(monkeypatch, tmp_path) -> None:
  _configure(tmp_path)
  _sources(tmp_path)
  (tmp_path / "src" / "common.hpp").write_text(
    "#pragma once\n#include <missing.hpp>\n")
  monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
  make = Make()
  make.preprocess()
  make.precompile()
  assert not make.errored
  assert b"missing.hpp" in make.output["compile"]["err"]
  seat = _seats(tmp_path, make)["plain"]
  assert seat.exists()
  assert not seat.with_name(seat.name + ".gch").exists()
