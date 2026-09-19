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
from pathlib import Path as PyPath
from types import SimpleNamespace

import pytest

from env import Path as LocalPath
from make import config
from make.compiler import Compiler
from make.config import BUILD_OUTPUT_PATH, LIBRARY_PATH
from make.initializer import Initializer
from make.linker import Linker

TABLE = {
    "glfw3": "glfw3",
    "mbedtls": ["mbedtls", "mbedcrypto", "mbedx509"],
    "sqlite": "sqlite3",
}
BLOCK = {"selector": "SR_LIBRARIES", "value": "SR_SYSTEM", "packages": TABLE}

STUB = """#!/bin/sh
kind=$1
shift
for name in "$@"; do
  case "$name" in
    absent) echo "Package absent was not found" >&2; exit 1 ;;
    spaced) printf -- '-I/usr/include/a\\ b ' ;;
  esac
  case "$kind" in
    --cflags) printf -- '-I/usr/include/%s ' "$name" ;;
    --libs)   printf -- '-l%s ' "$name" ;;
  esac
done
echo
"""


@pytest.fixture
def pkgconfig(tmp_path, monkeypatch) -> None:
    seat = tmp_path / "bin"
    seat.mkdir()
    script = seat / "pkg-config"
    script.write_text(STUB)
    script.chmod(0o755)
    monkeypatch.setenv("PATH", f"{seat}{os.pathsep}{os.environ['PATH']}")
    monkeypatch.delenv("PKG_CONFIG", raising=False)
    config._query.cache_clear()
    yield
    config._query.cache_clear()


def test_the_token_elects_the_zone(monkeypatch) -> None:
    monkeypatch.setattr(config, "SYSTEM", BLOCK)
    assert config.system({"SR_LIBRARIES": "SR_SYSTEM"}) == TABLE
    assert config.system({"SR_LIBRARIES": "SR_VENDORED"}) is None
    assert config.system({}) is None
    monkeypatch.setattr(config, "SYSTEM", {})
    assert config.system({"SR_LIBRARIES": "SR_SYSTEM"}) is None


def test_packages_flatten_in_table_order() -> None:
    assert config.packages(TABLE) == [
        "glfw3", "mbedtls", "mbedcrypto", "mbedx509", "sqlite3"]
    assert config.packages(None) == []


def test_no_package_asks_pkg_config_nothing(monkeypatch) -> None:
    def refuse(*_args, **_keywords):
        raise AssertionError("pkg-config ran off the system zone")

    monkeypatch.setattr("make.config.subprocess.run", refuse)
    config._query.cache_clear()
    assert config.flags("--cflags", []) == []


def _compiler(tmp_path, table) -> Compiler:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.flags = []
    make.roots = [tmp_path / "src"]
    make.include_paths = {tmp_path / "submodules" / "SDL" / "include"}
    make.libraries = []
    make.builds = []
    make.precompiled = []
    make.system = table
    make.toolchain = {
        "compiler": "g++", "standard": ["-std=c++23"], "compile_flags": [],
        "link_flags": [], "library_flags": ["-fPIC"],
        "output": {"kind": "native", "directory": None},
    }
    return Compiler(make)


def test_the_compile_takes_the_distributions_include_roots(
    tmp_path, pkgconfig
) -> None:
    command = _compiler(tmp_path, TABLE)._build_command(
        PyPath("a.cpp"), PyPath("a.o"))
    for package in config.packages(TABLE):
        assert f"-I/usr/include/{package}" in command
    vendored = "-I" + str(tmp_path / "submodules" / "SDL" / "include")
    assert command.index("-I/usr/include/glfw3") < command.index(vendored)
    plain = _compiler(tmp_path, None)._build_command(
        PyPath("a.cpp"), PyPath("a.o"))
    assert not [arg for arg in plain if arg.startswith("-I/usr/include")]


def _linker(tmp_path, table) -> Linker:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(str(tmp_path))
    make.output = {"builds": {}, "libraries": {}}
    make.system = table
    make.prebuilt = None
    make.zones = {PyPath("libs/debug"), PyPath("libs/release")}
    make.toolchain = {
        "compiler": "g++", "link_flags": [],
        "output": {"kind": "native", "directory": None},
    }
    return Linker(make)


def test_the_link_takes_the_distributions_libraries(
    tmp_path, pkgconfig
) -> None:
    zone = tmp_path / str(LIBRARY_PATH) / "debug" / "sqlite"
    zone.mkdir(parents=True)
    (zone / "libsqlite3.a").write_bytes(b"")
    (tmp_path / str(BUILD_OUTPUT_PATH)).mkdir(parents=True)

    command = _linker(tmp_path, TABLE)._build_command(
        {"name": "Island"}, ["main.o"])
    for package in config.packages(TABLE):
        assert f"-l{package}" in command
    assert not [arg for arg in command if arg.startswith("-L")]
    assert not [arg for arg in command if arg.startswith("-Wl,-rpath")]
    assert str(zone) not in " ".join(command)


def test_a_marked_archive_becomes_its_package(
    tmp_path, pkgconfig, monkeypatch
) -> None:
    import make.linker as module
    monkeypatch.setattr(module, "LIBRARY_ARCHIVES", [
        PyPath("build/outputs/src/store/libstore.a"),
        PyPath("<prebuilt>/sqlite/libsqlite3.a"),
    ])
    own = tmp_path / "build" / "outputs" / "src" / "store"
    own.mkdir(parents=True)
    (own / "libstore.a").write_bytes(b"")

    command = _linker(tmp_path, TABLE)._library_command(
        "thing", ["thing.o"], PyPath("cartridges/thing"))
    assert str(own / "libstore.a") in command
    assert "-lsqlite3" in command
    assert not [arg for arg in command if "<prebuilt>" in arg]


def test_a_marked_archive_with_no_row_is_named_aloud(
    tmp_path, pkgconfig, monkeypatch, caplog
) -> None:
    import make.linker as module
    monkeypatch.setattr(module, "LIBRARY_ARCHIVES",
                        [PyPath("<prebuilt>/imgui/libimgui.a")])
    command = _linker(tmp_path, TABLE)._library_command(
        "thing", ["thing.o"], PyPath("cartridges/thing"))
    assert not [arg for arg in command if "imgui" in arg]
    assert "imgui" in caplog.text


def test_a_refused_package_is_named(pkgconfig) -> None:
    assert config.flags("--cflags", ["glfw3"]) == ["-I/usr/include/glfw3"]
    with pytest.raises(config.Refused) as refusal:
        config.flags("--libs", ["glfw3", "absent", "sqlite3"])
    assert "absent" in str(refusal.value)
    assert "sqlite3" not in str(refusal.value)


def test_an_escaped_space_stays_one_argument(pkgconfig) -> None:
    assert config.flags("--cflags", ["spaced"]) == [
        "-I/usr/include/a b", "-I/usr/include/spaced"]


def test_a_refusal_errors_the_build_at_init(pkgconfig, caplog) -> None:
    make: SimpleNamespace = SimpleNamespace(
        system={"glfw3": "glfw3", "mbedtls": ["mbedtls", "absent"]},
        errored=False)
    Initializer._provisioned(make)
    assert make.errored
    assert "absent" in caplog.text
    plain = SimpleNamespace(system=None, errored=False)
    Initializer._provisioned(plain)
    assert not plain.errored


def test_a_rowless_library_directory_is_named_aloud(
    tmp_path, pkgconfig, caplog
) -> None:
    for zone in ("debug", "release"):
        for name in ("rmlui", "sqlite"):
            seat = tmp_path / str(LIBRARY_PATH) / zone / name
            seat.mkdir(parents=True)
            (seat / f"lib{name}.a").write_bytes(b"")
    (tmp_path / str(BUILD_OUTPUT_PATH)).mkdir(parents=True)
    command = _linker(tmp_path, TABLE)._build_command(
        {"name": "Island"}, ["main.o"])
    assert not [arg for arg in command if "rmlui" in arg]
    assert caplog.text.count("No system package for rmlui") == 1
    assert "sqlite" not in caplog.text
