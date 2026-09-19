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

from env import Path as LocalPath
from make.config import LIBRARY_PATH
from make.initializer import Initializer
from make.linker import Linker

VARIANTS = [
    {"selector": "SR_LOGGER_BACKEND",
     "options": {"/log4cpp": "SR_LOG4CPP", "/spdlog": "SR_SPDLOG"}},
]


def _initializer(tmp_path, logger) -> Initializer:
    init = Initializer.__new__(Initializer)
    init.root = LocalPath(str(tmp_path))
    init.prebuilt = PyPath("libs/debug")
    init.zones = {PyPath("libs/debug")}
    init._project_cfg = {
        "tokens": {"SR_LOGGER_BACKEND": logger},
        "variants": VARIANTS,
        "includes": ["submodules/log4cpp/include", "submodules/spdlog/include"],
    }
    return init


def _lay(tmp_path) -> dict:
    seats = {}
    for name, file_ in (("log4cpp", "liblog4cpp.so"), ("sqlite", "libsqlite3.a")):
        seat = tmp_path / str(LIBRARY_PATH) / "debug" / name
        (seat / "include").mkdir(parents=True)
        (seat / file_).write_bytes(b"")
        seats[name] = seat
    for name in ("log4cpp", "spdlog"):
        (tmp_path / "submodules" / name / "include").mkdir(parents=True)
    return seats


def test_a_compiled_out_library_leaves_the_include_roots(tmp_path) -> None:
    seats = _lay(tmp_path)
    init = _initializer(tmp_path, "SR_SPDLOG")
    init._generate_include_paths()
    roots = {str(root) for root in init.include_paths}
    assert str(tmp_path / "submodules" / "spdlog" / "include") in roots
    assert str(tmp_path / "submodules" / "log4cpp" / "include") not in roots
    assert str(seats["log4cpp"] / "include") not in roots
    assert str(seats["sqlite"] / "include") in roots
    init = _initializer(tmp_path, "SR_LOG4CPP")
    init._generate_include_paths()
    assert str(seats["log4cpp"] / "include") in {
        str(root) for root in init.include_paths}


def test_a_compiled_out_library_leaves_link_discovery(tmp_path) -> None:
    seats = _lay(tmp_path)
    (tmp_path / "build" / "outputs").mkdir(parents=True)
    for logger, linked in (("SR_SPDLOG", False), ("SR_LOG4CPP", True)):
        make = _initializer(tmp_path, logger)
        make.output = {"builds": {}, "libraries": {}}
        make.system = None
        make.toolchain = {"compiler": "g++", "link_flags": [],
                          "output": {"kind": "native", "directory": None}}
        libs = Linker(make)._collect_libs()
        assert (seats["log4cpp"] / "liblog4cpp.so" in libs) is linked
        assert seats["sqlite"] / "libsqlite3.a" in libs
