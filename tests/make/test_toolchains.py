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

import pytest
import yaml

from make import config
from make.config import PROJECT_CONFIG_PATH, toolchain
from make.initializer import Initializer
from make.make import Make

WEB = {
    "compiler": "emcc",
    "archiver": "emar",
    "link_flags": [],
    "library_flags": ["-fPIC"],
    "output": {"kind": "web", "directory": "build/web"},
    "delivery": "static",
}


def test_default_toolchain_is_the_native_policy() -> None:
    chain = toolchain(None)
    assert chain["compiler"] == config.COMPILER
    assert chain["archiver"] == config.ARCHIVER
    assert chain["standard"] == list(config.STANDARD)
    assert chain["link_flags"] == list(config.LINK_FLAGS)
    assert chain["library_flags"] == list(config.LIBRARY_FLAGS)
    assert chain["output"] == {
        "kind": "native", "directory": None, "binary": ".out", "library": ".so"}
    assert chain["delivery"] == "dynamic"


def test_named_toolchain_overrides_only_stated_policy(monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {"web": WEB})
    chain = toolchain("web")
    assert chain["compiler"] == "emcc"
    assert chain["archiver"] == "emar"
    assert chain["link_flags"] == []
    assert chain["output"] == {
        "kind": "web", "directory": "build/web", "binary": ".out", "library": ".so"}
    assert chain["delivery"] == "static"
    assert chain["standard"] == list(config.STANDARD)


def test_unknown_toolchain_is_the_native_default(monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {})
    assert toolchain("web") == toolchain(None)


def _project(tmp_path, platform: str, builds: bool = False) -> None:
    document = {
        "compiler": "g++",
        "standard": "c++23",
        "tokens": {
            "platform": {"SR_PLATFORM": platform},
            "values": {"SR_POSIX": 9, "SR_WEB": 20},
        },
        "toolchain": {"selector": "SR_PLATFORM", "options": {"SR_WEB": "web"}},
        "toolchains": {"web": WEB},
    }
    if builds:
        document["builds"] = [
            {"name": "page", "path": "src", "output_destinations": []}]
    path = tmp_path / PROJECT_CONFIG_PATH
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as file:
        yaml.safe_dump(document, file)


def _source(tmp_path) -> None:
    source = tmp_path / "src"
    source.mkdir(exist_ok=True)
    (source / "main.cpp").write_text("int main() { return 0; }\n")


def test_selector_picks_the_web_toolchain(monkeypatch, tmp_path) -> None:
    _source(tmp_path)
    _project(tmp_path, "SR_WEB")
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    monkeypatch.setattr(config, "TOOLCHAINS", {"web": WEB})
    make = Make()
    assert make.toolchain_name == "web"
    assert make.toolchain["compiler"] == "emcc"
    assert make.toolchain["output"]["kind"] == "web"


def test_posix_stays_the_native_toolchain(monkeypatch, tmp_path) -> None:
    _source(tmp_path)
    _project(tmp_path, "SR_POSIX")
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    monkeypatch.setattr(config, "TOOLCHAINS", {"web": WEB})
    make = Make()
    assert make.toolchain_name is None
    assert make.toolchain["compiler"] == config.COMPILER


def test_web_toolchain_takes_its_own_object_zone(monkeypatch, tmp_path) -> None:
    _source(tmp_path)
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    monkeypatch.setattr(config, "TOOLCHAINS", {"web": WEB})
    _project(tmp_path, "SR_POSIX")
    native = Make().zone
    _project(tmp_path, "SR_WEB")
    web = Make().zone
    assert web != native
    assert Make().zone == web


@pytest.mark.skipif(shutil.which("emcc") is None, reason="emsdk not on PATH")
def test_web_toolchain_compiles_and_links_a_wasm(monkeypatch, tmp_path) -> None:
    _source(tmp_path)
    _project(tmp_path, "SR_WEB", builds=True)
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    monkeypatch.setattr(config, "TOOLCHAINS", {"web": WEB})
    make = Make()
    make.preprocess()
    make.precompile()
    make.compile()
    make.link()
    make.copy_output()
    assert not make.errored
    web = tmp_path / "build" / "web"
    for artifact in ("page.wasm", "page.js", "page.html"):
        assert (web / artifact).exists(), artifact


from pathlib import Path as PyPath
from types import SimpleNamespace

from env import Path as LocalPath
from make.linker import Linker

WINDOWS = {
    "compiler": "x86_64-w64-mingw32-g++",
    "archiver": "x86_64-w64-mingw32-ar",
    "link_flags": ["-static", "-Wl,--allow-multiple-definition"],
    "library_flags": [],
    "imports": ["ws2_32", "psapi"],
    "prebuilt": {"SR_DEBUG": "libs/windows/debug",
                 "SR_RELEASE": "libs/windows/release"},
    "output": {"kind": "native", "directory": "build/windows",
               "binary": ".exe", "library": ".dll"},
}


def test_windows_toolchain_resolves_imports_zone_and_suffixes(monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    chain = toolchain("windows")
    assert chain["compiler"] == "x86_64-w64-mingw32-g++"
    assert chain["archiver"] == "x86_64-w64-mingw32-ar"
    assert chain["imports"] == ["ws2_32", "psapi"]
    assert chain["prebuilt"] == WINDOWS["prebuilt"]
    assert chain["output"] == WINDOWS["output"]
    assert chain["library_flags"] == []
    native = toolchain(None)
    assert native["imports"] == []
    assert native["prebuilt"] == {}
    assert native["output"] == {
        "kind": "native", "directory": None, "binary": ".out", "library": ".so"}


def _windows_linker(tmp_path, monkeypatch) -> Linker:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    for zone in ("libs/debug", "libs/windows/debug"):
        seat = tmp_path / zone / "glfw3"
        seat.mkdir(parents=True)
        (seat / "libglfw3.a").write_bytes(b"")
    make = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {"builds": {}, "libraries": {}}
    make.system = None
    make.toolchain = toolchain("windows")
    make.prebuilt = PyPath("libs/windows/debug")
    make.zones = {PyPath("libs/debug"), PyPath("libs/release"),
                  PyPath("libs/windows/debug"), PyPath("libs/windows/release")}
    return Linker(make)


def test_windows_link_command(tmp_path, monkeypatch) -> None:
    linker = _windows_linker(tmp_path, monkeypatch)
    command = linker._build_command({"name": "Island"}, ["main.o"])
    assert command[0] == "x86_64-w64-mingw32-g++"
    assert command[2].endswith("build/windows/outputs/Island.exe")
    assert "build/outputs/" not in command[2]
    assert "-static" in command
    assert "-Wl,--export-dynamic" not in command
    assert command[-2:] == ["-lws2_32", "-lpsapi"]
    assert command.index("main.o") < command.index("-lglfw3") < command.index("-lws2_32")
    seats = [arg for arg in command if arg.startswith("-L")]
    assert seats == [f"-L{tmp_path}/libs/windows/debug/glfw3"]


def test_windows_library_command(tmp_path, monkeypatch) -> None:
    linker = _windows_linker(tmp_path, monkeypatch)
    command = linker._library_command("pad", ["pad.o"], PyPath("cartridges/pad"))
    assert command[:3] == ["x86_64-w64-mingw32-g++", "-shared", "-o"]
    assert command[3].endswith("build/windows/outputs/cartridges/pad/libpad.dll")
    assert command[-2:] == ["-lws2_32", "-lpsapi"]
    assert Linker._shared("libpad.dll")


def test_windows_staging_root(tmp_path, monkeypatch) -> None:
    linker = _windows_linker(tmp_path, monkeypatch)
    assert str(linker.outputs()) == "build/windows/outputs"
    staged = linker.staged(PyPath("src/conduit"), "libconduit.a")
    assert str(staged).endswith("build/windows/outputs/src/conduit/libconduit.a")
    binary = linker.binary({"name": "Island"})
    assert str(binary).endswith("build/windows/outputs/Island.exe")
    assert binary.parent.is_dir()
    linker.make.toolchain = toolchain(None)
    assert str(linker.outputs()) == "build/outputs"


def _zoned_project(tmp_path, platform: str) -> None:
    document = {
        "compiler": "g++",
        "standard": "c++23",
        "tokens": {
            "platform": {"SR_PLATFORM": platform},
            "profile": {"SR_PROFILE": "SR_DEBUG"},
            "values": {"SR_POSIX": 9, "SR_WINDOWS": 21, "SR_DEBUG": 1,
                       "SR_RELEASE": 2},
        },
        "prebuilt": {"selector": "SR_PROFILE",
                     "options": {"SR_DEBUG": "libs/debug",
                                 "SR_RELEASE": "libs/release"}},
        "toolchain": {"selector": "SR_PLATFORM",
                      "options": {"SR_WINDOWS": "windows"}},
        "toolchains": {"windows": WINDOWS},
    }
    path = tmp_path / PROJECT_CONFIG_PATH
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as file:
        yaml.safe_dump(document, file)


def test_windows_toolchain_carries_its_own_zone_map(monkeypatch, tmp_path) -> None:
    _source(tmp_path)
    monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    monkeypatch.setattr(config, "PREBUILT", {
        "selector": "SR_PROFILE",
        "options": {"SR_DEBUG": "libs/debug", "SR_RELEASE": "libs/release"}})
    every = {PyPath("libs/debug"), PyPath("libs/release"),
             PyPath("libs/windows/debug"), PyPath("libs/windows/release")}
    _zoned_project(tmp_path, "SR_WINDOWS")
    make = Make()
    assert make.toolchain_name == "windows"
    assert make.prebuilt == PyPath("libs/windows/debug")
    assert make.zones == every
    _zoned_project(tmp_path, "SR_POSIX")
    make = Make()
    assert make.toolchain_name is None
    assert make.prebuilt == PyPath("libs/debug")
    assert make.zones == every


def test_clang_is_asked_of_the_binary_not_the_spelling(monkeypatch) -> None:
    config.clang.cache_clear()
    assert config.clang("clang++-18")
    assert config.clang("emcc")
    assert not config.clang("no-such-compiler-on-this-desk")
    assert config.clang("g++") is ("clang" in subprocess.run(
        ["g++", "--version"], capture_output=True, text=True).stdout.splitlines()[0])
    cross = shutil.which("x86_64-w64-mingw32-g++")
    if cross:
        answer = subprocess.run(
            [cross, "--version"], capture_output=True, text=True).stdout
        assert config.clang("x86_64-w64-mingw32-g++") is ("clang" in answer.splitlines()[0])


def _bundles(delivery: str | None) -> Initializer:
    init = Initializer.__new__(Initializer)
    init._project_cfg = {
        "tokens": {"SR_CARTRIDGE": "SR_MEADOW", "SR_NONE": 1, "SR_MEADOW": 2},
        "libraries": [
            {"name": "meadow", "path": "cartridges/.test/meadow"},
            {"name": "pad", "path": "cartridges/.core/pad"},
            {"name": "shell", "path": "src/shell", "archive": True},
        ],
        "statics": [{"selector": "SR_CARTRIDGE",
                     "options": {"cartridges/.test/meadow": "SR_MEADOW"}}],
    }
    init.toolchain = toolchain(None) if delivery is None else {
        **toolchain(None), "delivery": delivery,
        "output": {"kind": "native", "directory": "build/windows",
                   "binary": ".exe", "library": ".dll"}}
    return init


def test_static_delivery_links_no_bundle_shared_library() -> None:
    names = [one["name"] for one in _bundles("static")._libraries()]
    assert names == ["meadow", "shell"]


def test_dynamic_delivery_keeps_every_library() -> None:
    names = [one["name"] for one in _bundles(None)._libraries()]
    assert names == ["meadow", "pad", "shell"]
    names = [one["name"] for one in _bundles("dynamic")._libraries()]
    assert names == ["meadow", "pad", "shell"]
