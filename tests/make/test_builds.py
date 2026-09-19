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
from pathlib import Path as PyPath
from types import SimpleNamespace

import pytest

from env import ROOT_DIRECTORY
from env import Path as LocalPath
from make.initializer import Initializer
from make.linker import Linker
from make.config import (
    BUILD_OBJECT_PATH,
    BUILD_OUTPUT_PATH,
    COMPILER,
    LOG_PATH,
)


def _builds() -> list[dict]:
    return [
        {
            "name": "Islands",
            "path": PyPath("src/island"),
            "links": [PyPath("src/common")],
            "destinations": ["."],
        },
        {
            "name": "probe",
            "path": PyPath("src/test/probe"),
            "links": [PyPath("src/common")],
            "destinations": [],
        },
    ]


def _make(tmp_path) -> SimpleNamespace:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.mirror = BUILD_OBJECT_PATH
    make.errored = False
    make.output = {"builds": {}}
    make.builds = _builds()
    make.libraries = []
    make.objects = {
        str(PyPath(directory) / file_)
        for directory, _, files in os.walk(tmp_path / BUILD_OBJECT_PATH)
        for file_ in files
    }
    return make


def _mirror(tmp_path, subtree: str, name: str) -> None:
    mirror = tmp_path / BUILD_OBJECT_PATH / subtree
    mirror.mkdir(parents=True, exist_ok=True)
    (mirror / name).write_bytes(b"")


def test_builds_parses_config_entries() -> None:
    init = Initializer.__new__(Initializer)
    init._project_cfg = {
        "builds": [
            {"name": "probe", "path": "src/test/probe", "links": ["src/common"]},
            {"name": "incomplete"},
        ]
    }
    builds = init._builds()
    assert len(builds) == 1
    assert builds[0]["name"] == "probe"
    assert builds[0]["path"] == PyPath("src/test/probe")
    assert builds[0]["links"] == [PyPath("src/common")]
    assert builds[0]["destinations"] == []


def test_no_builds_section_means_none() -> None:
    init = Initializer.__new__(Initializer)
    init._project_cfg = {}
    assert init._builds() == []


def test_build_links_entry_and_links_objects(tmp_path) -> None:
    _mirror(tmp_path, "src/island", "wip.island.o")
    _mirror(tmp_path, "src/common", "wip.time.o")
    _mirror(tmp_path, "src/test/probe", "wip.probe.o")
    make = _make(tmp_path)
    linker = Linker(make)
    commands = []
    linker._run_link_command = lambda command: (commands.append(command), (0, b""))[1]
    linker.builds()
    island, probe = (" ".join(command) for command in commands)
    assert "wip.island.o" in island and "wip.time.o" in island
    assert "wip.probe.o" not in island
    assert "wip.probe.o" in probe and "wip.time.o" in probe
    assert "wip.island.o" not in probe


def test_build_command_is_executable(tmp_path) -> None:
    make = _make(tmp_path)
    linker = Linker(make)
    command = linker._build_command({"name": "probe"}, ["a.o", "b.o"])
    assert COMPILER in command
    assert "-shared" not in command
    assert "-Wl,--export-dynamic" in command
    assert str(LocalPath(str(tmp_path)) / BUILD_OUTPUT_PATH / "probe.out") in command
    assert "a.o" in command and "b.o" in command


def test_statics_fold_into_the_first_build_only(tmp_path) -> None:
    _mirror(tmp_path, "src/island", "wip.island.o")
    _mirror(tmp_path, "src/test/probe", "wip.probe.o")
    _mirror(tmp_path, "cartridges/.test/meadow", "wip.cartridge.o")
    make = _make(tmp_path)
    make.libraries = [
        {"name": "meadow", "path": PyPath("cartridges/.test/meadow"), "static": True}]
    linker = Linker(make)
    commands = []
    linker._run_link_command = lambda command: (commands.append(command), (0, b""))[1]
    linker.builds()
    island, probe = (" ".join(command) for command in commands)
    assert "wip.cartridge.o" in island
    assert "wip.cartridge.o" not in probe


def test_build_link_failure_errors_the_build(tmp_path) -> None:
    _mirror(tmp_path, "src/island", "wip.island.o")
    make = _make(tmp_path)
    linker = Linker(make)
    linker._run_link_command = lambda command: (1, b"collision")
    linker.builds()
    assert make.errored


def test_write_logs_emits_per_build_files(tmp_path) -> None:
    import os

    from make.make import Make

    make = Make.__new__(Make)
    make.root = tmp_path
    make.output = {
        "compile": {"out": b"", "err": b""},
        "libraries": {},
        "builds": {"probe": {"out": b"ok", "command": b"cmd"}},
    }
    os.makedirs(tmp_path / LOG_PATH, exist_ok=True)
    make._write_logs()
    assert (tmp_path / LOG_PATH / "link.probe.out.log").exists()
    assert (tmp_path / LOG_PATH / "link.probe.command.log").exists()


def test_probe_runs() -> None:
    probe = ROOT_DIRECTORY / BUILD_OUTPUT_PATH / "probe.out"
    if not probe.exists():
        pytest.skip(
            "build/outputs/probe.out missing; build it: python3 -m make")
    result = subprocess.run([str(probe)], capture_output=True, text=True)
    assert result.returncode == 0
    assert "Probe running" in result.stdout
