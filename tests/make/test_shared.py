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
import stat
from pathlib import Path as PyPath
from types import SimpleNamespace

from env import Path as LocalPath
from make import config
from make.config import LINK_FLAGS, delivery, toolchain
from make.initializer import Initializer
from make.linker import Linker
from make.make import Make

BLOCK = {"selector": "SR_LINKAGE",
         "options": {"SR_STATIC": "static", "SR_SHARED": "shared"}}
WINDOWS = {
    "compiler": "x86_64-w64-mingw32-g++",
    "archiver": "x86_64-w64-mingw32-ar",
    "link_flags": ["-static", "-Wl,--allow-multiple-definition"],
    "library_flags": [],
    "imports": ["ws2_32", "psapi"],
    "delivery": BLOCK,
    "engine": "islands",
    "prebuilt": {"SR_DEBUG": "libs/windows/debug",
                 "SR_RELEASE": "libs/windows/release"},
    "output": {"kind": "native", "directory": "build/windows",
               "binary": ".exe", "library": ".dll"},
}
MIRROR = "build/objects/zone"
OBJECTS = [
    "src/island/wip.island.o", "src/island/graphics/wip.gfx.o",
    "src/island/wip.load.static.o", "src/cartridge/wip.load.o",
    "src/topics/wip.bus.o", "src/topics/durable/wip.durable.o",
    "src/reef/wip.reef.o", "src/test/durable/wip.main.o",
    "cartridges/.test/meadow/wip.cartridge.o",
]
STUB = """#!/bin/sh
printf '%s\\n' "$@" >> "$LEDGER"
printf '%s\\n' -- >> "$LEDGER"
if [ "$1" = rcs ]; then : > "$2"; exit 0; fi
while [ $# -gt 0 ]; do
  case "$1" in
    -o) : > "$2"; shift;;
    -Wl,--out-implib,*) : > "${1#-Wl,--out-implib,}";;
  esac
  shift
done
"""


def test_a_delivery_word_stands_and_a_block_reads_its_token() -> None:
    assert delivery("static", {}) == "static"
    assert delivery(BLOCK, {"SR_LINKAGE": "SR_SHARED"}) == "shared"
    assert delivery(BLOCK, {"SR_LINKAGE": "SR_STATIC"}) == "static"
    assert delivery(BLOCK, {}) == "static"
    assert delivery(BLOCK, {"SR_LINKAGE": "SR_ODD"}) == "static"


def test_the_windows_toolchain_resolves_shared_and_names_the_engine(
        monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    assert toolchain("windows")["delivery"] == "static"
    chain = toolchain("windows", {"SR_LINKAGE": "SR_SHARED"})
    assert chain["delivery"] == "shared"
    assert chain["engine"] == "islands"
    assert toolchain(None, {"SR_LINKAGE": "SR_SHARED"})["delivery"] == "dynamic"
    assert toolchain(None)["engine"] is None


VOCABULARY = {
    "path": "manifest.yaml",
    "section": "engine",
    "folds": {"axis": "platform", "section": "platforms"},
    "axes": {"platform": {"token": "SR_PLATFORM",
                          "values": {"posix": "SR_POSIX",
                                     "windows": "SR_WINDOWS"}}},
}


def _bundles(delivery_word: str, tmp_path) -> Initializer:
    init = Initializer.__new__(Initializer)
    init.root = LocalPath(str(tmp_path))
    init.errored = False
    init.fold = "windows"
    init._project_cfg = {
        "tokens": {"SR_CARTRIDGE": "SR_MEADOW", "SR_NONE": 1, "SR_MEADOW": 2},
        "manifest": VOCABULARY,
        "libraries": [
            {"name": "meadow", "path": "cartridges/.test/meadow"},
            {"name": "pad", "path": "cartridges/.core/pad"},
            {"name": "quay", "path": "cartridges/quay"},
            {"name": "shell", "path": "src/shell", "archive": True},
        ],
        "statics": [{"selector": "SR_CARTRIDGE",
                     "options": {"cartridges/.test/meadow": "SR_MEADOW"}}],
    }
    init.toolchain = {**toolchain(None), "delivery": delivery_word}
    for bundle, stated in (("cartridges/.test/meadow", "[posix, windows]"),
                           ("cartridges/.core/pad", "[posix]"),
                           ("cartridges/quay", None)):
        seat = tmp_path / bundle
        seat.mkdir(parents=True)
        body = "name: x\n"
        if stated:
            body += f"engine:\n  platform: {stated}\n"
        (seat / "manifest.yaml").write_text(body)
    return init


def test_static_delivery_links_the_fold_and_the_archives_alone(
        tmp_path) -> None:
    names = [one["name"] for one in _bundles("static", tmp_path)._libraries()]
    assert names == ["meadow", "shell"]


def test_shared_delivery_links_the_bundles_that_ship_this_fold(
        tmp_path) -> None:
    names = [one["name"] for one in _bundles("shared", tmp_path)._libraries()]
    assert names == ["meadow", "shell"]
    init = _bundles("shared", tmp_path.parent / "posix")
    init.fold = "posix"
    assert [one["name"] for one in init._libraries()] == [
        "meadow", "pad", "shell"]


def test_the_native_delivery_links_every_bundle(tmp_path) -> None:
    names = [one["name"] for one in _bundles("dynamic", tmp_path)._libraries()]
    assert names == ["meadow", "pad", "quay", "shell"]


def _stub(tmp_path, monkeypatch) -> PyPath:
    compiler = tmp_path / "compiler"
    compiler.write_text(STUB)
    compiler.chmod(compiler.stat().st_mode | stat.S_IEXEC)
    monkeypatch.setenv("LEDGER", str(tmp_path / "ledger"))
    return compiler


def _make(tmp_path, monkeypatch, shared: bool = True) -> SimpleNamespace:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    compiler = _stub(tmp_path, monkeypatch)
    seat = tmp_path / "libs/windows/debug/glfw3"
    seat.mkdir(parents=True)
    (seat / "libglfw3.a").write_bytes(b"")
    for one in OBJECTS:
        (tmp_path / MIRROR / one).parent.mkdir(parents=True, exist_ok=True)
        (tmp_path / MIRROR / one).write_bytes(b"")
    make = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.mirror = MIRROR
    make.objects = {str(tmp_path / MIRROR / one) for one in OBJECTS}
    make.errored = False
    make.output = {"builds": {}, "libraries": {}}
    make.system = None
    tokens = {"SR_LINKAGE": "SR_SHARED" if shared else "SR_STATIC"}
    make.toolchain = {**toolchain("windows", tokens),
                      "compiler": str(compiler), "archiver": str(compiler)}
    make.prebuilt = PyPath("libs/windows/debug")
    make.zones = {PyPath("libs/debug"), PyPath("libs/windows/debug")}
    make.builds = [
        {"name": "Island", "path": PyPath("src/island"), "main": "island.cpp",
         "links": [PyPath("src/cartridge"), PyPath("src/topics")],
         "destinations": ["build"], "host": True},
        {"name": "Reef", "path": PyPath("src/reef"),
         "links": [PyPath("src/cartridge"), PyPath("src/topics")],
         "destinations": ["build"], "main": None, "host": True},
        {"name": "durable", "path": PyPath("src/test/durable"),
         "links": [PyPath("src/topics/durable"), PyPath("src/topics")],
         "destinations": [], "main": None, "host": False},
    ]
    make.libraries = [
        {"name": "durable", "path": PyPath("src/topics/durable"),
         "archive": True, "static": False},
        {"name": "meadow", "path": PyPath("cartridges/.test/meadow"),
         "static": True, "archive": False},
    ]
    return make


def _commands(tmp_path) -> list[list[str]]:
    ledger = (tmp_path / "ledger").read_text().split("\n--\n")
    return [one.split("\n") for one in ledger if one.strip()]


def _names(command: list[str]) -> list[str]:
    return [PyPath(arg).name for arg in command if arg.endswith(".o")]


def test_the_engine_links_first_over_the_engine_and_the_fold(
        tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch)
    Linker(make).link()
    assert not make.errored
    archive, engine, island, reef, durable = _commands(tmp_path)
    assert archive[0] == "rcs" and archive[1].endswith("libdurable.a")
    assert engine[:2] == ["-shared", "-o"]
    assert engine[2].endswith("build/windows/outputs/islands.dll")
    assert engine[3].endswith("build/windows/outputs/libislands.dll.a")
    assert engine[3].startswith("-Wl,--out-implib,")
    assert engine[4:6] == ["-static", "-Wl,--allow-multiple-definition"]
    assert _names(engine) == [
        "wip.load.o", "wip.gfx.o", "wip.load.static.o", "wip.bus.o",
        "wip.cartridge.o"]
    assert engine[-3:] == ["-lglfw3", "-lws2_32", "-lpsapi"]
    assert "islands" in make.output["libraries"]
    assert (tmp_path / "build/windows/outputs/islands.dll").exists()
    assert (tmp_path / "build/windows/outputs/libislands.dll.a").exists()
    assert island[2:4] == ["-static", "-Wl,--allow-multiple-definition"]
    assert island[1].endswith("Island.exe") and reef[1].endswith("Reef.exe")
    assert durable[1].endswith("durable.exe")


def test_a_bundle_links_against_the_same_import_library(
        tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch)
    command = Linker(make)._library_command(
        "meadow", ["one.o"], PyPath("cartridges/.test/meadow"))
    assert command[:3] == [make.toolchain["compiler"], "-shared", "-o"]
    assert command[3].endswith(
        "build/windows/outputs/cartridges/.test/meadow/libmeadow.dll")
    assert command[4:7] == [
        "-static", "-Wl,--allow-multiple-definition", "one.o"]
    assert command[7].endswith("libislands.dll.a")
    assert command[-2:] == ["-lws2_32", "-lpsapi"]


def test_a_native_bundle_line_is_byte_identical(tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch, shared=False)
    command = Linker(make)._library_command(
        "meadow", ["one.o"], PyPath("cartridges/.test/meadow"))
    assert command[4:] == ["one.o", "-lws2_32", "-lpsapi"]


def test_each_host_links_its_own_against_the_import_library(
        tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch)
    Linker(make).link()
    _, _, island, reef, durable = _commands(tmp_path)
    implib = str(tmp_path / "build/windows/outputs/libislands.dll.a")
    assert _names(island) == ["wip.island.o"]
    assert _names(reef) == ["wip.reef.o"]
    assert _names(durable) == ["wip.main.o", "wip.durable.o", "wip.bus.o"]
    assert not any(arg.endswith("libislands.dll.a") for arg in durable)
    for command in (island, reef):
        assert implib in command
        objects = max(at for at, arg in enumerate(command) if arg.endswith(".o"))
        assert objects < command.index(implib) < command.index("-lglfw3")
        assert command[-2:] == ["-lws2_32", "-lpsapi"]


def test_static_linkage_keeps_the_windows_line_as_it_was(
        tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch, shared=False)
    Linker(make).link()
    archive, island, reef, durable = _commands(tmp_path)
    assert archive[0] == "rcs"
    assert not (tmp_path / "build/windows/outputs/islands.dll").exists()
    assert _names(island) == [
        "wip.gfx.o", "wip.island.o", "wip.load.static.o", "wip.load.o",
        "wip.bus.o", "wip.cartridge.o"]
    assert _names(reef) == ["wip.reef.o", "wip.load.o", "wip.bus.o"]
    assert _names(durable) == ["wip.main.o", "wip.durable.o", "wip.bus.o"]
    for command in (island, reef, durable):
        assert not any(arg.endswith("libislands.dll.a") for arg in command)


def test_the_native_line_is_byte_identical(tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch)
    make.toolchain = toolchain(None, {"SR_LINKAGE": "SR_SHARED"})
    make.prebuilt = None
    linker = Linker(make)
    assert not linker._sharing()
    command = linker._build_command(make.builds[0], ["main.o"])
    assert command == [
        config.COMPILER, "-o",
        str(tmp_path / "build/outputs/Island.out"), *LINK_FLAGS, "main.o"]


def test_the_engine_library_is_delivered_beside_the_first_host(
        tmp_path, monkeypatch) -> None:
    make = _make(tmp_path, monkeypatch)
    Linker(make).link()
    real = Make.__new__(Make)
    real.toolchain = make.toolchain
    real.builds = make.builds
    real.root = make.root
    real.linker = Linker(real)
    copied = []
    real._copy_artifact = lambda source, name, destinations: copied.append(
        (source.name, name, destinations))
    real._copy_engine(make.root / "build/windows/outputs")
    assert copied == [("islands.dll", "islands.dll", ["build"])]
    real.toolchain = toolchain(None)
    copied.clear()
    real._copy_engine(make.root / "build/outputs")
    assert copied == []
