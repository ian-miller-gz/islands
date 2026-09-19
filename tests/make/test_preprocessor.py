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
import types
from pathlib import Path as PyPath

from make.config import PROJECT_CONFIG_PATH
from make.preprocessor import Preprocessor


def _make(root: PyPath) -> types.SimpleNamespace:
    return types.SimpleNamespace(root=root)


def test_tokens_emitted_unquoted(tmp_path: PyPath) -> None:
    config = tmp_path / PROJECT_CONFIG_PATH
    config.parent.mkdir(parents=True)
    config.write_text(
        "tokens:\n"
        "  SR_GFX_BACKEND: SR_GFX_VULKAN\n"
        "directives:\n"
        '  PROJECT_NAME: "Islands"\n'
    )

    header = Preprocessor(_make(tmp_path)).generate()
    content = header.read_text()

    assert "#define SR_GFX_BACKEND SR_GFX_VULKAN" in content
    assert '#define PROJECT_NAME "Islands"' in content


def test_missing_config_is_noop(tmp_path: PyPath) -> None:
    header = Preprocessor(_make(tmp_path)).generate()
    assert header.name == "preprocessor.hpp"


GROUPED = (
    "tokens:\n"
    "  network:\n"
    "    SR_NETWORK_BACKEND: SR_UNIX\n"
    "  values:\n"
    "    SR_UNIX: 3\n"
    "  SR_FLAT: 9\n"
    "directives:\n"
    "  project:\n"
    '    PROJECT_NAME: "Islands"\n'
)


def test_grouped_entries_render_axis_headers(tmp_path: PyPath) -> None:
    config = tmp_path / PROJECT_CONFIG_PATH
    config.parent.mkdir(parents=True)
    config.write_text(GROUPED)

    catalog = Preprocessor(_make(tmp_path)).generate()
    generated = catalog.parent

    network = (generated / "network.hpp").read_text()
    assert "#define SR_NETWORK_BACKEND SR_UNIX" in network
    assert "SR_UNIX 3" not in network
    assert "#define SR_UNIX 3" in (generated / "values.hpp").read_text()
    project = (generated / "project.hpp").read_text()
    assert '#define PROJECT_NAME "Islands"' in project

    content = catalog.read_text()
    for line in ("#define SR_NETWORK_BACKEND SR_UNIX", "#define SR_UNIX 3",
                 "#define SR_FLAT 9", '#define PROJECT_NAME "Islands"'):
        assert line in content


def test_unchanged_group_is_not_restamped(tmp_path: PyPath) -> None:
    config = tmp_path / PROJECT_CONFIG_PATH
    config.parent.mkdir(parents=True)
    config.write_text(GROUPED)
    processor = Preprocessor(_make(tmp_path))
    generated = processor.generate().parent
    values = (generated / "values.hpp").stat().st_mtime_ns

    config.write_text(GROUPED.replace("SR_NETWORK_BACKEND: SR_UNIX",
                                      "SR_NETWORK_BACKEND: SR_TCP"))
    processor.generate()
    network = (generated / "network.hpp").read_text()
    assert "#define SR_NETWORK_BACKEND SR_TCP" in network
    assert (generated / "values.hpp").stat().st_mtime_ns == values
