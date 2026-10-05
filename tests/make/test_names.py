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
# A toolchain's `names:` — the platform's own spelling of a build's binary
# (Island delivered as islands on Windows, where the binary is the command)
# — and the `named:` directive a build declares its delivered name through.
from pathlib import Path as PyPath
from types import SimpleNamespace

from env import Path as LocalPath
from make import config
from make.config import toolchain
from make.initializer import Initializer
from make.linker import Linker
from make.make import Make

WINDOWS = {
    "compiler": "x86_64-w64-mingw32-g++",
    "archiver": "x86_64-w64-mingw32-ar",
    "link_flags": ["-static"],
    "library_flags": [],
    "names": {"Island": "islands"},
    "output": {"kind": "native", "directory": "build/windows",
               "binary": ".exe", "library": ".dll"},
}

BUILDS = [
    {"name": "Island", "path": "src/island", "output_destinations": ["build"],
     "named": {"group": "engine", "directive": "ISLAND_NAME"}},
    {"name": "Reef", "path": "src/reef", "output_destinations": ["build"],
     "named": {"group": "engine", "directive": "REEF_NAME"}},
]


def _config(platform: str) -> dict:
    return {
        "tokens": {"platform": {"SR_PLATFORM": platform},
                   "values": {"SR_POSIX": 9, "SR_WINDOWS": 10}},
        "toolchain": {"selector": "SR_PLATFORM", "options": {"SR_WINDOWS": "windows"}},
        "toolchains": {"windows": WINDOWS},
        "directives": {"engine": {"ENGINE_NAME": "Islands"}},
        "builds": BUILDS,
    }


def _initializer() -> Initializer:
    init = Initializer.__new__(Initializer)
    init.errored = False
    return init


def _linker(tmp_path, monkeypatch, chain) -> Linker:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    make = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {"builds": {}, "libraries": {}}
    make.system = None
    make.toolchain = toolchain(chain)
    make.prebuilt = PyPath("libs/debug")
    make.zones = {PyPath("libs/debug")}
    return Linker(make)


def test_toolchain_resolves_its_names(monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    assert toolchain("windows")["names"] == {"Island": "islands"}
    assert toolchain(None)["names"] == {}


def test_named_binary_is_spelled_by_the_toolchain(tmp_path, monkeypatch) -> None:
    linker = _linker(tmp_path, monkeypatch, "windows")
    assert str(linker.binary({"name": "Island"})).endswith("build/windows/outputs/islands.exe")
    assert str(linker.binary({"name": "Reef"})).endswith("build/windows/outputs/Reef.exe")
    command = linker._build_command({"name": "Island"}, ["main.o"])
    assert command[2].endswith("build/windows/outputs/islands.exe")


def test_native_binary_keeps_the_build_name(tmp_path, monkeypatch) -> None:
    linker = _linker(tmp_path, monkeypatch, None)
    assert str(linker.binary({"name": "Island"})).endswith("build/outputs/Island.out")


def test_named_directive_follows_the_spelling(monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    windows = _config("SR_WINDOWS")
    _initializer()._overlay(windows)
    assert windows["directives"]["engine"]["ISLAND_NAME"] == "islands"
    assert windows["directives"]["engine"]["REEF_NAME"] == "Reef"
    assert windows["directives"]["engine"]["ENGINE_NAME"] == "Islands"
    posix = _config("SR_POSIX")
    _initializer()._overlay(posix)
    assert posix["directives"]["engine"]["ISLAND_NAME"] == "Island"
    assert posix["directives"]["engine"]["REEF_NAME"] == "Reef"


def test_named_without_group_or_directive_errors() -> None:
    init = _initializer()
    broken = _config("SR_POSIX")
    broken["builds"] = [{"name": "Island", "path": "src/island", "named": {"group": "engine"}}]
    init._overlay(broken)
    assert init.errored
    assert "ISLAND_NAME" not in broken.get("directives", {}).get("engine", {})


def test_delivery_copies_the_spelled_binary(tmp_path, monkeypatch) -> None:
    monkeypatch.setattr(config, "TOOLCHAINS", {"windows": WINDOWS})
    outputs = tmp_path / "build/windows/outputs"
    outputs.mkdir(parents=True)
    (outputs / "islands.exe").write_bytes(b"island")
    (outputs / "Reef.exe").write_bytes(b"reef")
    make = Make.__new__(Make)
    make.root = LocalPath(str(tmp_path))
    make.errored = False
    make.toolchain = toolchain("windows")
    make.builds = [
        {"name": "Island", "destinations": ["build"]},
        {"name": "Reef", "destinations": ["build"]},
    ]
    make.libraries = []
    make.linker = SimpleNamespace(
        spelled=lambda build: make.toolchain["names"].get(build["name"], build["name"]),
        outputs=lambda: PyPath("build/windows/outputs"))
    copied = []
    make._copy_artifact = lambda src, name, destinations: copied.append((src.name, name, destinations))
    make._copy_engine = lambda outputs: None
    make.copy_output()
    assert copied == [("islands.exe", "islands.exe", ["build"]),
                      ("Reef.exe", "Reef.exe", ["build"])]
