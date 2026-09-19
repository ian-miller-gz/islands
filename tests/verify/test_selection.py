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
from verify.runner import configuration, selection

TABLE = [
    {"prefix": "notes/"},
    {"prefix": "tests/make/", "pytest": ["tests/make"]},
    {"prefix": "cartridges/.test/messages/",
     "pytest": ["cartridges/.test/messages/tests"], "golds": ["messages"]},
    {"prefix": "src/island/graphics/", "golds": ["pixels"], "matrix": ["sdl"]},
]


def test_leaf_scoped_diff_narrows() -> None:
    narrowed = selection(["cartridges/.test/messages/serve.cpp"], TABLE)
    assert narrowed == {
        "pytest": ["cartridges/.test/messages/tests"], "golds": ["messages"],
        "matrix": []}


def test_cross_cutting_diff_falls_through_to_full() -> None:
    changes = ["cartridges/.test/messages/serve.cpp", "src/common/types.hpp"]
    assert selection(changes, TABLE) is None


def test_documentation_diff_selects_nothing() -> None:
    narrowed = selection(["notes/coding.md"], TABLE)
    assert narrowed == {"pytest": [], "golds": [], "matrix": []}


def test_matches_union_across_changes() -> None:
    changes = ["tests/make/test_make.py", "cartridges/.test/messages/serve.cpp"]
    narrowed = selection(changes, TABLE)
    assert narrowed == {
        "pytest": ["cartridges/.test/messages/tests", "tests/make"],
        "golds": ["messages"], "matrix": []}


def test_matrix_column_names_relevant_cells() -> None:
    narrowed = selection(["src/island/graphics/render.cpp"], TABLE)
    assert narrowed == {"pytest": [], "golds": ["pixels"], "matrix": ["sdl"]}


def test_project_table_reads_and_shapes() -> None:
    project = configuration()
    assert {"build", "pytest", "golds"} <= set(project["stages"])
    for entry in project["changed"]:
        assert entry["prefix"].endswith(("/", ".yaml", "."))
        assert isinstance(entry.get("pytest", []), list)
        assert isinstance(entry.get("golds", []), list)
