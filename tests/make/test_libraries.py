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
from make.linker import Linker
from make.config import (
    BUILD_OBJECT_PATH,
    BUILD_OUTPUT_PATH,
    COMPILER,
    LOG_PATH,
)


def _library() -> dict:
    return {
        "name": "demo",
        "path": PyPath("cartridges/.test/demo"),
        "destinations": ["."],
    }


def test_libraries_parses_config_entries() -> None:
    init = Initializer.__new__(Initializer)
    init._project_cfg = {
        "libraries": [
            {"name": "demo", "path": "cartridges/.test/demo", "output_destinations": ["."]},
            {"name": "incomplete"},
        ]
    }
    libraries = init._libraries()
    assert len(libraries) == 1
    assert libraries[0]["name"] == "demo"
    assert libraries[0]["path"] == PyPath("cartridges/.test/demo")
    assert libraries[0]["destinations"] == ["."]


def test_no_libraries_section_means_none() -> None:
    init = Initializer.__new__(Initializer)
    init._project_cfg = {}
    assert init._libraries() == []


def test_library_sources_compile_position_independent() -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.libraries = [_library()]
    make.roots = []
    make.include_paths = set()
    compiler = Compiler(make)
    assert compiler._flags(PyPath("cartridges/.test/demo/demo.cpp")) == [
        "-fPIC", "-fno-gnu-unique"]
    assert compiler._flags(PyPath("src/island.cpp")) == []
    command = compiler._build_command(
        LocalPath("cartridges/.test/demo/demo.cpp"),
        LocalPath("build/cartridges/.test/demo/wip.demo.o"),
        ["-fPIC"],
    )
    assert "-fPIC" in command


def test_library_link_command_is_shared(tmp_path) -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    linker = Linker(make)
    path = PyPath("cartridges/demo")
    command = linker._library_command("demo", ["a.o", "b.o"], path)
    assert COMPILER in command
    assert "-shared" in command
    staged = LocalPath(str(tmp_path)) / BUILD_OUTPUT_PATH / str(path)
    assert str(staged / "libdemo.so") in command
    assert "a.o" in command and "b.o" in command


def test_same_named_targets_stage_under_their_own_paths(tmp_path) -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    linker = Linker(make)
    core = linker._library_command(
        "monitor", ["a.o"], PyPath("cartridges/.core/launcher/monitor"))
    nested = linker._library_command(
        "monitor", ["b.o"], PyPath("cartridges/castaway/monitor"))
    seats = [
        part for command in (core, nested)
        for part in command if part.endswith("libmonitor.so")]
    assert len(seats) == 2 and seats[0] != seats[1]
    assert all(PyPath(seat).name == "libmonitor.so" for seat in seats)


def test_collect_objects_walks_build_mirror(tmp_path) -> None:
    objects_dir = tmp_path / BUILD_OBJECT_PATH / "cartridges" / ".test" / "demo"
    objects_dir.mkdir(parents=True)
    (objects_dir / "wip.demo.o").write_bytes(b"")
    (objects_dir / "wip.stale.o").write_bytes(b"")
    (objects_dir / "demo.cpp").write_text("")
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.mirror = BUILD_OBJECT_PATH
    make.objects = {str(objects_dir / "wip.demo.o")}
    linker = Linker(make)
    objects = linker._collect_objects(PyPath("cartridges/.test/demo"))
    assert [PyPath(o).name for o in objects] == ["wip.demo.o"]


def test_write_library_logs_records_per_target(tmp_path) -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {"libraries": {}}
    linker = Linker(make)
    linker._write_library_logs("demo", ["g++", "-shared"], b"ok")
    assert b"ok" in make.output["libraries"]["demo"]["out"]
    assert b"-shared" in make.output["libraries"]["demo"]["command"]


def test_library_subtree_joins_metadata(tmp_path) -> None:
    init = Initializer.__new__(Initializer)
    init.root = LocalPath(str(tmp_path))
    init.mirror = BUILD_OBJECT_PATH
    init.metadata = {}
    init.includes = set()
    init.objects = set()
    init._project_cfg = {}
    source = tmp_path / "cartridges" / "apps" / "demo"
    source.mkdir(parents=True)
    (source / "demo.cpp").write_text("")
    init._parse_metadata(init.root / "cartridges" / "apps" / "demo")
    assert "demo.cpp" in init.metadata["cartridges"]["apps"]["demo"]


def test_write_logs_emits_per_library_files(tmp_path) -> None:
    import os

    from make.make import Make

    make = Make.__new__(Make)
    make.root = tmp_path
    make.output = {
        "compile": {"out": b"", "err": b""},
        "link": {"out": b"", "err": b""},
        "libraries": {"demo": {"out": b"ok", "command": b"cmd"}},
    }
    os.makedirs(tmp_path / LOG_PATH, exist_ok=True)
    make._write_logs()
    assert (tmp_path / LOG_PATH / "link.demo.out.log").exists()
    assert (tmp_path / LOG_PATH / "link.demo.command.log").exists()


def test_library_link_appends_existing_archives(tmp_path, monkeypatch) -> None:
    import make.linker as linker_module
    (tmp_path / "libdemo.a").write_bytes(b"!<arch>\n")
    monkeypatch.setattr(
        linker_module, "LIBRARY_ARCHIVES",
        [PyPath("libdemo.a"), PyPath("gone.a")])
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    linker = Linker(make)
    command = linker._library_command("demo", ["a.o"], PyPath("cartridges/demo"))
    archive = str(LocalPath(str(tmp_path)) / "libdemo.a")
    assert archive in command
    assert command.index("a.o") < command.index(archive)
    assert not any("gone.a" in part for part in command)


def test_archive_target_runs_the_archiver(tmp_path, monkeypatch) -> None:
    import make.linker as linker_module
    calls = []
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.mirror = PyPath("objects")
    make.objects = set()
    make.output = {"libraries": {}, "builds": {}}
    make.errored = False
    linker = Linker(make)
    out_dir = tmp_path / BUILD_OUTPUT_PATH / "src" / "store"
    out_dir.mkdir(parents=True)
    stale = out_dir / "libstore.a"
    stale.write_bytes(b"stale")
    monkeypatch.setattr(linker, "_collect_objects", lambda path: ["a.o"])
    monkeypatch.setattr(
        linker, "_run_link_command",
        lambda command: (calls.append(command), (0, b""))[1])
    linker._archive({"name": "store", "path": PyPath("src/store")})
    assert not stale.exists()
    assert calls and calls[0][0:2] == ["ar", "rcs"]
    assert calls[0][2].endswith("libstore.a") and "a.o" in calls[0]
