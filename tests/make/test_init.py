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
import subprocess
import sys
from pathlib import Path as PyPath

from env import Path as LocalPath
from env import ROOT_DIRECTORY
from make.initializer import Initializer
from make.config import BUILD_OBJECT_PATH


def _config(selected: str) -> dict:
    return {
        "tokens": {"SR_GFX_BACKEND": selected},
        "variants": [
            {
                "selector": "SR_GFX_BACKEND",
                "options": {
                    "island/graphics/backend/vulkan": "SR_GFX_VULKAN",
                    "island/graphics/backend/sdl-gpu": "SR_GFX_SDL3",
                },
            }
        ],
    }


def test_is_active_variant_selects_configured_option() -> None:
    init = Initializer()
    base = "/some/project/island/graphics/backend"
    vulkan = LocalPath(f"{base}/vulkan/impl.cpp")
    sdl = LocalPath(f"{base}/sdl-gpu/impl.cpp")
    other = LocalPath("/some/project/island/graphics/render.cpp")

    init._project_cfg = _config("SR_GFX_VULKAN")
    assert init._is_active_variant(vulkan) is True
    assert init._is_active_variant(sdl) is False
    assert init._is_active_variant(other) is True

    init._project_cfg = _config("SR_GFX_SDL3")
    assert init._is_active_variant(vulkan) is False
    assert init._is_active_variant(sdl) is True
    assert init._is_active_variant(other) is True


def _shared_config(selected: str) -> dict:
    return {
        "tokens": {"SR_NET": selected},
        "variants": [
            {
                "selector": "SR_NET",
                "options": {
                    "network/sockets/backend/unix": "SR_UNIX",
                    "network/sockets/backend/inet": ["SR_TCP", "SR_TCP6"],
                },
            }
        ],
    }


def test_variant_option_list_selects_for_any_member() -> None:
    init = Initializer()
    inet = LocalPath("/p/network/sockets/backend/inet/sessions.cpp")
    unix = LocalPath("/p/network/sockets/backend/unix/sessions.cpp")

    for token in ("SR_TCP", "SR_TCP6"):
        init._project_cfg = _shared_config(token)
        assert init._is_active_variant(inet) is True
        assert init._is_active_variant(unix) is False

    init._project_cfg = _shared_config("SR_UNIX")
    assert init._is_active_variant(inet) is False
    assert init._is_active_variant(unix) is True


def test_include_roots_come_from_the_config(tmp_path: PyPath, caplog) -> None:
    init = Initializer.__new__(Initializer)
    init.root = LocalPath(str(tmp_path))
    (tmp_path / "submodules" / "stb").mkdir(parents=True)
    init._project_cfg = {
        "includes": ["submodules", "submodules/stb", "submodules/vulkan/include"]
    }
    init._generate_include_paths()
    roots = {str(path) for path in init.include_paths}
    assert str(tmp_path / "submodules") in roots
    assert str(tmp_path / "submodules" / "stb") in roots
    assert str(tmp_path / "submodules" / "vulkan" / "include") in roots
    assert str(tmp_path / "build" / "include") in roots
    assert "submodules/vulkan/include" in caplog.text


def test_declared_roots_consolidate_headers_beneath_them() -> None:
    init = Initializer.__new__(Initializer)
    init.root = LocalPath("/p")
    init._project_cfg = {"roots": ["cartridges/.test", "cartridges"]}
    init.includes = {
        LocalPath("/p/cartridges/.test/meadow/state.hpp"),
        LocalPath("/p/cartridges/.test/roster/roster.hpp"),
        LocalPath("/p/cartridges/resident/resident.hpp"),
        LocalPath("/p/src/console/console.hpp"),
        LocalPath("/p/src/common.hpp"),
    }
    assert init._roots() == [
        LocalPath("/p/cartridges"),
        LocalPath("/p/cartridges/.test"),
        LocalPath("/p/src"),
        LocalPath("/p/src/console"),
    ]


def test_no_declared_roots_keeps_header_directories() -> None:
    init = Initializer.__new__(Initializer)
    init.root = LocalPath("/p")
    init._project_cfg = {}
    init.includes = {
        LocalPath("/p/src/console/console.hpp"),
        LocalPath("/p/src/console/selection.hpp"),
    }
    assert init._roots() == [LocalPath("/p/src/console")]


def test_tokens_flatten_reads_groups_and_flat_alike() -> None:
    config = {"tokens": {"network": {"SR_NETWORK_BACKEND": "SR_UNIX"},
                         "SR_FLAT": 9}}
    assert Initializer._tokens(config) == {
        "SR_NETWORK_BACKEND": "SR_UNIX", "SR_FLAT": 9}


def test_grouped_tokens_drive_variant_selection() -> None:
    init = Initializer()
    config = _shared_config("ignored")
    config["tokens"] = {"net": {"SR_NET": "SR_TCP"}}
    init._project_cfg = config
    inet = LocalPath("/p/network/sockets/backend/inet/sessions.cpp")
    assert init._is_active_variant(inet) is True


def test_no_variants_means_all_active() -> None:
    init = Initializer()
    init._project_cfg = {}
    path = LocalPath("/x/island/graphics/backend/vulkan/impl.cpp")
    assert init._is_active_variant(path) is True


def test_project_config_reads_real_make_yaml() -> None:
    init = Initializer()
    init.root = ROOT_DIRECTORY
    config = init._project_config()
    tokens = Initializer._tokens(config)
    variants = config.get("variants", [])
    assert tokens and variants
    for variant in variants:
        selected = tokens.get(variant["selector"])
        offered = []
        for value in variant["options"].values():
            offered += value if isinstance(value, list) else [value]
        assert selected in offered or selected in tokens


def test_parse_files_uses_string_keys(tmp_path: PyPath) -> None:
    init = Initializer.__new__(Initializer)
    init.root = tmp_path
    init.mirror = BUILD_OBJECT_PATH
    init.metadata = {}
    init.includes = set()
    init.objects = set()
    (tmp_path / "a.cpp").write_text("")
    init._parse_files(tmp_path, [PyPath("a.cpp")])
    assert list(init.metadata.keys()) == ["a.cpp"]
    assert all(type(key) is str for key in init.metadata)
    assert init.objects == {str(init._object(PyPath("a.cpp")))}


def test_parse_files_gates_file_granular_variants(tmp_path) -> None:
    directory = tmp_path / "src" / "island"
    directory.mkdir(parents=True)
    for name in ("island.cpp", "web.cpp"):
        (directory / name).write_text("")
    init = Initializer()
    init.root = LocalPath(str(tmp_path))
    init.mirror = LocalPath("build/objects/zone")
    init.metadata = {}
    init.includes = set()
    init.objects = set()
    init._project_cfg = {
        "tokens": {"SR_PLATFORM": "SR_WEB"},
        "variants": [
            {
                "selector": "SR_PLATFORM",
                "options": {
                    "island/island.cpp": "SR_POSIX",
                    "island/web.cpp": "SR_WEB",
                },
            }
        ],
    }
    files = [PyPath("island.cpp"), PyPath("web.cpp")]
    init._parse_files(LocalPath(str(directory)), files)
    parsed = init.metadata["src"]["island"]
    assert "web.cpp" in parsed
    assert "island.cpp" not in parsed


def test_root_walks_to_a_make_config_without_git(tmp_path: PyPath) -> None:
    (tmp_path / "configs").mkdir()
    (tmp_path / "configs" / "make.yaml").write_text("")
    deep = tmp_path / "src" / "deep"
    deep.mkdir(parents=True)
    probe = "from env import ROOT_DIRECTORY; print(ROOT_DIRECTORY)"
    found = subprocess.run([sys.executable, "-c", probe], cwd=deep,
                           capture_output=True, text=True, check=True)
    assert PyPath(found.stdout.strip()) == tmp_path.resolve()
