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
import shutil
import subprocess
from pathlib import Path as PyPath
from types import SimpleNamespace

import pytest

from env import Path as LocalPath
from make.linker import Linker
from make.config import BUILD_OUTPUT_PATH, LIBRARY_PATH, LIBRARY_PRUNES

HELPER = "int helper() { return 7; }\n"
CALLER = "extern int helper();\nint main() { return helper() == 7 ? 0 : 1; }\n"


def test_collect_libs_and_write_logs(tmp_path) -> None:
    libs_dir = tmp_path / str(LIBRARY_PATH)
    libs_dir.mkdir(parents=True, exist_ok=True)
    (libs_dir / "libfoo.so").write_bytes(b'')
    (libs_dir / "libbar.a").write_bytes(b'')
    sdk_dir = tmp_path / str(LIBRARY_PRUNES[0]) / "vulkan"
    sdk_dir.mkdir(parents=True, exist_ok=True)
    (sdk_dir / "libvulkan.so").write_bytes(b'')

    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.includes = set()
    make.include_paths = set()
    make.output = {'builds': {}}

    linker = Linker(make)
    libs = linker._collect_libs()
    assert {lib.name for lib in libs} == {"libfoo.so", "libbar.a"}

    cmd: list = ['echo', 'dummy']
    linker._write_build_logs('Islands', cmd, b'ok')
    assert b'ok' in make.output['builds']['Islands']['out']


def test_prebuilt_zone_selects_discovery_and_the_archive_mark(tmp_path, monkeypatch) -> None:
    libs_dir = tmp_path / str(LIBRARY_PATH)
    for zone, name in (("debug", "libdev.a"), ("release", "libopt.a")):
        (libs_dir / zone / "x").mkdir(parents=True)
        (libs_dir / zone / "x" / name).write_bytes(b'')
    (libs_dir / "shared").mkdir()
    (libs_dir / "shared" / "libboth.so").write_bytes(b'')

    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.includes = set()
    make.include_paths = set()
    make.output = {'builds': {}, 'libraries': {}}
    make.zones = {PyPath("libs/debug"), PyPath("libs/release")}
    make.prebuilt = PyPath("libs/release")
    import make.linker as module
    monkeypatch.setattr(module, "LIBRARY_ARCHIVES",
                        [PyPath("<prebuilt>/x/libopt.a"), PyPath("build/outputs/libplain.a")])

    linker = Linker(make)
    assert {lib.name for lib in linker._collect_libs()} == {"libopt.a", "libboth.so"}
    assert linker._archives() == [PyPath("libs/release/x/libopt.a"), PyPath("build/outputs/libplain.a")]

    make.prebuilt = None
    assert {lib.name for lib in Linker(make)._collect_libs()} == {"libboth.so"}
    assert Linker(make)._archives() == [PyPath("build/outputs/libplain.a")]


def test_nested_target_objects_stay_out_of_the_parent_link(tmp_path) -> None:
    mirror = "objects/zone"
    parent = PyPath("cartridges/game")
    child = parent / "monitor"
    for path, stem in ((parent, "game"), (child, "watch")):
        directory = tmp_path / mirror / str(path)
        directory.mkdir(parents=True, exist_ok=True)
        (directory / f"wip.{stem}.o").write_bytes(b'')

    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.mirror = mirror
    make.libraries = [
        {"name": "game", "path": parent},
        {"name": "watch", "path": child},
    ]
    make.builds = []
    make.objects = {
        str(tmp_path / mirror / str(parent) / "wip.game.o"),
        str(tmp_path / mirror / str(child) / "wip.watch.o"),
    }

    linker = Linker(make)
    assert [PyPath(o).name for o in linker._collect_objects(parent)] == [
        "wip.game.o"]
    assert [PyPath(o).name for o in linker._collect_objects(child)] == [
        "wip.watch.o"]


def test_preloads_render_into_the_web_link_alone(tmp_path) -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {'builds': {}}
    linker = Linker(make)
    build = {"name": "Island", "preloads": ["assets", "cartridges/pack"]}
    web = {
        "compiler": "emcc", "link_flags": ["-sASYNCIFY"],
        "output": {"kind": "web", "directory": "build/web"},
    }
    command = linker._web_command(build, ["main.o"], web)
    assert command[:2] == ["emcc", "-o"]
    assert command[2].endswith("build/web/Island.html")
    assert command[3:] == [
        "-sASYNCIFY",
        "--preload-file", "assets@/assets",
        "--preload-file", "cartridges/pack@/cartridges/pack",
        "main.o",
    ]


def test_native_link_ignores_preloads(tmp_path) -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {'builds': {}}
    make.toolchain = {
        "compiler": "g++", "link_flags": [], "output": {"kind": "out"},
    }
    linker = Linker(make)
    libs_dir = tmp_path / str(LIBRARY_PATH)
    libs_dir.mkdir(parents=True, exist_ok=True)
    build = {"name": "Island", "preloads": ["assets"]}
    command = linker._build_command(build, ["main.o"])
    assert "--preload-file" not in command
    assert command[2].endswith("Island.out")


def _object(tmp_path, stem: str, source: str) -> str:
    unit = tmp_path / f"{stem}.cpp"
    unit.write_text(source)
    out = tmp_path / f"{stem}.o"
    subprocess.run(
        ["g++", "-fPIC", "-c", str(unit), "-o", str(out)], check=True)
    return str(out)


@pytest.mark.skipif(shutil.which("g++") is None, reason="no g++ on PATH")
def test_only_a_shared_library_earns_a_runtime_search_path(tmp_path) -> None:
    helper = _object(tmp_path, "helper", HELPER)
    caller = _object(tmp_path, "caller", CALLER)
    libs = tmp_path / str(LIBRARY_PATH)
    archive = libs / "release" / "helper"
    shared = libs / "debug" / "helper"
    archive.mkdir(parents=True)
    shared.mkdir(parents=True)
    subprocess.run(
        ["ar", "rcs", str(archive / "libhelper.a"), helper], check=True)
    subprocess.run(
        ["g++", "-shared", "-o", str(shared / "libhelper.so"), helper],
        check=True)
    (tmp_path / str(BUILD_OUTPUT_PATH)).mkdir(parents=True)

    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {"builds": {}}
    make.toolchain = {
        "compiler": "g++", "link_flags": [], "output": {"kind": "native"},
    }
    make.zones = {PyPath("libs/debug"), PyPath("libs/release")}

    commands = {}
    for zone, seat in (("release", archive), ("debug", shared)):
        make.prebuilt = PyPath(f"libs/{zone}")
        linker = Linker(make)
        command = linker._build_command({"name": "Island"}, [caller])
        commands[zone] = command
        assert "-L" + str(seat) in command
        assert "-lhelper" in command
        assert linker._run_link_command(command)[0] == 0

    rpaths = {
        zone: [arg for arg in command if arg.startswith("-Wl,-rpath")]
        for zone, command in commands.items()
    }
    assert rpaths["release"] == []
    assert rpaths["debug"] == ["-Wl,-rpath=" + str(shared)]
