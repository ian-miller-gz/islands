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

from make.compiler import Compiler
from make.config import BUILD_OBJECT_PATH
from make.make import Make


def _make(tmp_path) -> Make:
    make = Make.__new__(Make)
    make.root = tmp_path
    make.mirror = BUILD_OBJECT_PATH
    make.flags = []
    make.libraries = []
    make.builds = []
    make.includes = set()
    make.roots = []
    make.include_paths = set()
    make.objects = set()
    make.prints = {}
    make.times = {}
    make.stamp = 0.0
    make.compiler = Compiler(make)
    return make


def _compiled(make: Make, source: str, dependencies: list[str]) -> None:
    record = make._object(PyPath(source)).with_suffix(".d")
    record.parent.mkdir(parents=True, exist_ok=True)
    target = make._object(PyPath(source)).name
    record.write_text(f"{target}: {' '.join(dependencies)}\n")


def test_old_schema_is_stale(tmp_path) -> None:
    make = _make(tmp_path)
    assert make._fresh(1.0, 0.5, PyPath("a.cpp")) is False
    assert make._fresh(None, 0.5, PyPath("a.cpp")) is False


def test_modified_source_is_stale(tmp_path) -> None:
    make = _make(tmp_path)
    entry = [1.0, make._fingerprint(PyPath("a.cpp"))]
    assert make._fresh(entry, 2.0, PyPath("a.cpp")) is False


def test_missing_dependency_record_is_stale(tmp_path) -> None:
    make = _make(tmp_path)
    entry = [2.0, make._fingerprint(PyPath("a.cpp"))]
    assert make._fresh(entry, 1.0, PyPath("a.cpp")) is False


def test_fingerprint_mismatch_is_stale(tmp_path) -> None:
    make = _make(tmp_path)
    _compiled(make, "a.cpp", [])
    entry = [2.0, make._fingerprint(PyPath("a.cpp"))]
    make.flags = ["-O2"]
    make.prints = {}
    assert make._fresh(entry, 1.0, PyPath("a.cpp")) is False


def test_newer_dependency_is_stale(tmp_path) -> None:
    make = _make(tmp_path)
    header = tmp_path / "a.hpp"
    header.write_text("")
    _compiled(make, "a.cpp", [str(header)])
    entry = [1.0, make._fingerprint(PyPath("a.cpp"))]
    os.utime(header, (5.0, 5.0))
    assert make._fresh(entry, 1.0, PyPath("a.cpp")) is False


def test_deleted_dependency_is_stale(tmp_path) -> None:
    make = _make(tmp_path)
    _compiled(make, "a.cpp", [str(tmp_path / "gone.hpp")])
    entry = [2.0, make._fingerprint(PyPath("a.cpp"))]
    assert make._fresh(entry, 1.0, PyPath("a.cpp")) is False


def test_current_source_is_fresh(tmp_path) -> None:
    make = _make(tmp_path)
    header = tmp_path / "a.hpp"
    header.write_text("")
    os.utime(header, (1.0, 1.0))
    _compiled(make, "a.cpp", [str(header)])
    entry = [2.0, make._fingerprint(PyPath("a.cpp"))]
    assert make._fresh(entry, 1.0, PyPath("a.cpp")) is True


def test_headers_check_only_their_own_stamp(tmp_path) -> None:
    make = _make(tmp_path)
    assert make._fresh([2.0, ""], 1.0, PyPath("a.hpp")) is True
    assert make._fresh([2.0, ""], 3.0, PyPath("a.hpp")) is False


def test_dependency_records_parse_continuations(tmp_path) -> None:
    make = _make(tmp_path)
    record = make._object(PyPath("a.cpp")).with_suffix(".d")
    record.parent.mkdir(parents=True, exist_ok=True)
    record.write_text("wip.a.o: /x/one.hpp \\\n /x/two.hpp\n")
    assert make._dependencies(PyPath("a.cpp")) == ["/x/one.hpp", "/x/two.hpp"]


def test_update_cache_stamps_schema(tmp_path) -> None:
    make = _make(tmp_path)
    make.stamp = 7.0
    make.cache = {"src": {}}
    make.metadata = {"src": {"a.cpp": 1.0, "a.hpp": 1.0}}
    make.cache["src"] = make.metadata["src"]
    make._update_cache()
    assert make.cache["src"]["a.cpp"] == [7.0, make._fingerprint(PyPath("src/a.cpp"))]
    assert make.cache["src"]["a.hpp"] == [7.0, ""]


def test_set_updated_files_threads_the_path(tmp_path) -> None:
    make = _make(tmp_path)
    header = tmp_path / "a.hpp"
    header.write_text("")
    os.utime(header, (1.0, 1.0))
    _compiled(make, "src/deep/a.cpp", [str(header)])
    fingerprint = make._fingerprint(PyPath("src/deep/a.cpp"))
    make.cache = {"src": {"deep": {"a.cpp": [2.0, fingerprint]}}}
    make.metadata = {"src": {"deep": {"a.cpp": 1.0}}}
    make._set_updated_files()
    assert make.metadata == {"src": {"deep": {}}}


def test_preprocessor_writes_only_on_change(tmp_path) -> None:
    from types import SimpleNamespace

    from make.config import PROJECT_CONFIG_PATH
    from make.preprocessor import Preprocessor

    config = tmp_path / PROJECT_CONFIG_PATH
    config.parent.mkdir(parents=True)
    config.write_text("tokens:\n  SR_GFX_BACKEND: SR_GFX_VULKAN\n")
    preprocessor = Preprocessor(SimpleNamespace(root=tmp_path))
    header = preprocessor.generate()
    os.utime(header, (1.0, 1.0))
    preprocessor.generate()
    assert os.path.getmtime(header) == 1.0
    config.write_text("tokens:\n  SR_GFX_BACKEND: SR_GFX_SDL3\n")
    preprocessor.generate()
    assert os.path.getmtime(header) > 1.0
