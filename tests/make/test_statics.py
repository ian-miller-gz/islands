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
from make.config import BUILD_OBJECT_PATH


def _config(selected: str) -> dict:
    return {
        "tokens": {"SR_CARTRIDGE": selected, "SR_NONE": 1, "SR_MEADOW": 2},
        "libraries": [
            {"name": "meadow", "path": "cartridges/.test/meadow", "output_destinations": ["cartridges/.test/meadow"]},
            {"name": "demo", "path": "cartridges/.test/demo", "output_destinations": ["cartridges/.test/demo"]},
        ],
        "statics": [
            {"selector": "SR_CARTRIDGE", "options": {"cartridges/.test/meadow": "SR_MEADOW"}},
        ],
    }


def _initializer(selected: str) -> Initializer:
    init = Initializer.__new__(Initializer)
    init._project_cfg = _config(selected)
    return init


def test_statics_marks_the_selected_library() -> None:
    libraries = _initializer("SR_MEADOW")._libraries()
    marks = {library["name"]: library["static"] for library in libraries}
    assert marks == {"meadow": True, "demo": False}


def test_no_selection_keeps_every_library_dynamic() -> None:
    libraries = _initializer("SR_NONE")._libraries()
    assert not any(library["static"] for library in libraries)


def test_static_sources_compile_with_binary_codegen() -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.libraries = _initializer("SR_MEADOW")._libraries()
    make.includes = set()
    make.include_paths = set()
    compiler = Compiler(make)
    assert compiler._flags(PyPath("cartridges/.test/meadow/cartridge.cpp")) == []
    assert compiler._flags(PyPath("cartridges/.test/demo/demo.cpp")) == [
        "-fPIC", "-fno-gnu-unique"]


def test_static_objects_fold_into_the_project_binary(tmp_path) -> None:
    mirror = tmp_path / BUILD_OBJECT_PATH / "cartridges" / ".test" / "meadow"
    mirror.mkdir(parents=True)
    (mirror / "wip.cartridge.o").write_bytes(b"")
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(tmp_path)
    make.mirror = BUILD_OBJECT_PATH
    make.objects = {str(mirror / "wip.cartridge.o")}
    make.libraries = [
        {"name": "meadow", "path": PyPath("cartridges/.test/meadow"), "static": True}]
    linker = Linker(make)
    assert str(mirror / "wip.cartridge.o") in linker._statics()


def test_static_library_link_is_skipped() -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath("/project")
    make.errored = False
    make.libraries = [
        {"name": "meadow", "path": PyPath("cartridges/.test/meadow"), "static": True}]
    linker = Linker(make)
    linked = []
    linker._library = lambda library: linked.append(library["name"])
    linker.libraries()
    assert linked == []


def _hosted_config(selected: str) -> dict:
    return {
        "tokens": {"SR_CARTRIDGE": selected, "SR_NONE": 1, "SR_MEADOW": 2},
        "libraries": [
            {"name": "meadow", "path": "cartridges/.test/meadow", "output_destinations": ["cartridges/.test/meadow"]},
            {"name": "scout", "path": "cartridges/.test/meadow/scout", "output_destinations": ["cartridges/.test/meadow/scout"]},
            {"name": "demo", "path": "cartridges/.test/demo", "output_destinations": ["cartridges/.test/demo"]},
        ],
        "statics": [
            {"selector": "SR_CARTRIDGE", "options": {"cartridges/.test/meadow": "SR_MEADOW"},
             "bundle": {"group": "engine", "directive": "SR_CARTRIDGE_BUNDLE"}},
            {"selector": "SR_CARTRIDGE", "host": "Reef",
             "options": {"cartridges/.test/meadow/scout": "SR_MEADOW"},
             "bundle": {"group": "engine", "directive": "SR_REEF_BUNDLE"}},
        ],
    }


def _hosted_initializer(selected: str) -> Initializer:
    init = Initializer.__new__(Initializer)
    init._project_cfg = _hosted_config(selected)
    return init


def test_a_hosted_fold_names_its_build_and_a_nested_bundle_answers_to_its_own_row() -> None:
    libraries = _hosted_initializer("SR_MEADOW")._libraries()
    folds = {library["name"]: (library["static"], library["folds"]) for library in libraries}
    assert folds == {"meadow": (True, None), "scout": (True, "Reef"), "demo": (False, None)}


def test_a_nested_bundle_unselected_stays_dynamic_under_a_folded_parent() -> None:
    config = _hosted_config("SR_MEADOW")
    config["statics"][1]["options"] = {"cartridges/.test/meadow/scout": "SR_NONE"}
    init = Initializer.__new__(Initializer)
    init._project_cfg = config
    marks = {library["name"]: library["static"] for library in init._libraries()}
    assert marks == {"meadow": True, "scout": False, "demo": False}


def test_every_selected_fold_declares_its_directive() -> None:
    config = _hosted_config("SR_MEADOW")
    init = Initializer.__new__(Initializer)
    init._project_cfg = config
    init.root = LocalPath("/project")
    init._overlay(config)
    assert config["directives"]["engine"] == {
        "SR_CARTRIDGE_BUNDLE": "cartridges/.test/meadow",
        "SR_REEF_BUNDLE": "cartridges/.test/meadow/scout"}


def test_hosted_objects_fold_into_the_named_build_only(tmp_path) -> None:
    meadow = tmp_path / BUILD_OBJECT_PATH / "cartridges" / ".test" / "meadow"
    scout = meadow / "scout"
    scout.mkdir(parents=True)
    (meadow / "cartridge.o").write_bytes(b"")
    (scout / "scout.o").write_bytes(b"")
    make: SimpleNamespace = SimpleNamespace()
    make.root = LocalPath(tmp_path)
    make.mirror = BUILD_OBJECT_PATH
    make.objects = {str(meadow / "cartridge.o"), str(scout / "scout.o")}
    make.libraries = [
        {"name": "meadow", "path": PyPath("cartridges/.test/meadow"), "static": True, "folds": None},
        {"name": "scout", "path": PyPath("cartridges/.test/meadow/scout"), "static": True, "folds": "Reef"}]
    make.builds = []
    linker = Linker(make)
    assert linker._statics("Island", True) == [str(meadow / "cartridge.o")]
    assert linker._statics("Reef", False) == [str(scout / "scout.o")]
    assert linker._statics() == [str(meadow / "cartridge.o")]
