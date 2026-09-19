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


def _sources(tmp_path) -> None:
    source = tmp_path / "src"
    source.mkdir()
    text = "#include <generated/net.hpp>\n#include <generated/values.hpp>\n"
    (source / "main.cpp").write_text(text + "int main() { return AXIS == BLUE; }\n")
    (source / "other.cpp").write_text(text + "int other() { return AXIS; }\n")
    (tmp_path / "cartridges" / "thing").mkdir(parents=True)


def _manifest(tmp_path, engine: str = "") -> None:
    path = tmp_path / "cartridges" / "thing" / "manifest.yaml"
    path.write_text("name: thing\n" + engine)


def _configure(tmp_path, pick: str = "NONE", axis: str = "RED") -> None:
    config = {
        "tokens": {
            "app": {"PICK": pick},
            "net": {"AXIS": axis},
            "gate": {"GATE": "ON"},
            "values": {"NONE": 1, "THING": 2, "RED": 1, "BLUE": 2, "ON": 3, "OFF": 4},
        },
        "manifest": {
            "path": "manifest.yaml",
            "section": "engine",
            "axes": {
                "axis": {"token": "AXIS", "values": {"red": "RED", "blue": "BLUE"}},
            },
            "keys": {
                "gate": {
                    "token": "GATE",
                    "values": {True: "ON", False: "OFF"},
                    "absent": "OFF",
                },
            },
        },
        "statics": [{"selector": "PICK", "options": {"cartridges/thing": "THING"}}],
    }
    path = tmp_path / PROJECT_CONFIG_PATH
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as file:
        yaml.safe_dump(config, file)


def _pending(metadata) -> int:
    count = 0
    for key, value in metadata.items():
        if isinstance(value, dict):
            count += _pending(value)
        elif key.endswith(".cpp"):
            count += 1
    return count


def _build(monkeypatch, tmp_path) -> tuple[Make, int]:
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    make = Make()
    make.preprocess()
    pending = _pending(make.metadata)
    make.compile()
    make.clear()
    return make, pending


def test_alternation_reuses_each_zones_objects(monkeypatch, tmp_path) -> None:
    _sources(tmp_path)
    _manifest(tmp_path)
    _configure(tmp_path, axis="RED")
    make, first = _build(monkeypatch, tmp_path)
    assert not make.errored and first == 2
    _configure(tmp_path, axis="BLUE")
    make, second = _build(monkeypatch, tmp_path)
    assert not make.errored and second == 2
    _configure(tmp_path, axis="RED")
    make, third = _build(monkeypatch, tmp_path)
    assert not make.errored and third == 0
    _configure(tmp_path, axis="BLUE")
    make, fourth = _build(monkeypatch, tmp_path)
    assert not make.errored and fourth == 0


def test_zones_key_by_configuration(monkeypatch, tmp_path) -> None:
    _sources(tmp_path)
    _manifest(tmp_path)
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    _configure(tmp_path, axis="RED")
    red = Make().zone
    assert Make().zone == red
    _configure(tmp_path, axis="BLUE")
    blue = Make().zone
    assert blue != red
    for zone in (red, blue):
        assert (tmp_path / "build" / "objects" / zone).is_dir()


def test_manifest_axes_drive_the_static_build(monkeypatch, tmp_path) -> None:
    _sources(tmp_path)
    _configure(tmp_path, pick="THING")
    header = tmp_path / "build" / "include" / "generated" / "net.hpp"
    _manifest(tmp_path, "engine:\n  axis: blue\n")
    make, pending = _build(monkeypatch, tmp_path)
    assert not make.errored and pending == 2
    assert "#define AXIS BLUE" in header.read_text()
    _manifest(tmp_path)
    make, _ = _build(monkeypatch, tmp_path)
    assert not make.errored
    assert "#define AXIS RED" in header.read_text()


def test_unknown_manifest_axis_fails_before_compiling(
    monkeypatch, tmp_path, caplog
) -> None:
    _sources(tmp_path)
    _configure(tmp_path, pick="THING")
    for engine in ("engine:\n  axis: purple\n", "engine:\n  colour: red\n"):
        _manifest(tmp_path, engine)
        monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
        make = Make()
        assert make.errored
        make.preprocess()
        make.compile()
        assert not (tmp_path / "build" / "include" / "generated").exists()
        assert not list((tmp_path / "build" / "objects").rglob("*.o"))
    assert "cartridges/thing" in caplog.text and "colour" in caplog.text


def test_dynamic_delivery_ignores_the_section(monkeypatch, tmp_path) -> None:
    _sources(tmp_path)
    _manifest(tmp_path, "engine:\n  axis: bogus\n")
    _configure(tmp_path, pick="NONE")
    make, pending = _build(monkeypatch, tmp_path)
    assert not make.errored and pending == 2
    header = tmp_path / "build" / "include" / "generated" / "net.hpp"
    assert "#define AXIS RED" in header.read_text()


def test_manifest_key_drives_the_tier(monkeypatch, tmp_path) -> None:
    _sources(tmp_path)
    _configure(tmp_path, pick="THING")
    header = tmp_path / "build" / "include" / "generated" / "gate.hpp"
    for manifest, tier in (("gate: true\n", "ON"), ("gate: false\n", "OFF"), ("", "OFF")):
        _manifest(tmp_path, manifest)
        make, _ = _build(monkeypatch, tmp_path)
        assert not make.errored
        assert f"#define GATE {tier}" in header.read_text()


def test_unknown_key_value_fails_before_compiling(
    monkeypatch, tmp_path, caplog
) -> None:
    _sources(tmp_path)
    _configure(tmp_path, pick="THING")
    _manifest(tmp_path, "gate: sometimes\n")
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    make = Make()
    assert make.errored
    assert "cartridges/thing" in caplog.text and "gate" in caplog.text


def test_dynamic_delivery_ignores_the_keys(monkeypatch, tmp_path) -> None:
    _sources(tmp_path)
    _manifest(tmp_path, "gate: false\n")
    _configure(tmp_path, pick="NONE")
    make, _ = _build(monkeypatch, tmp_path)
    assert not make.errored
    header = tmp_path / "build" / "include" / "generated" / "gate.hpp"
    assert "#define GATE ON" in header.read_text()
