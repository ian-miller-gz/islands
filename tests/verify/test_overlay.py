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
from pathlib import Path

from verify import runner
from verify.runner import configuration, merge


def test_merge_recurses_appends_replaces() -> None:
    base = {"stages": {"pytest": {"roots": ["one/tests"], "command": ["x"]}},
            "changed": [{"prefix": "one/"}], "scalar": 1}
    overlay = {"stages": {"pytest": {"roots": ["tool/tests"]}},
               "changed": [{"prefix": "tool/"}], "scalar": 2}
    merge(base, overlay)
    assert base["stages"]["pytest"]["roots"] == ["one/tests", "tool/tests"]
    assert base["stages"]["pytest"]["command"] == ["x"]
    assert [entry["prefix"] for entry in base["changed"]] == ["one/", "tool/"]
    assert base["scalar"] == 2


def test_overlay_lane_appends_and_skips_the_project_config(
        tmp_path, monkeypatch) -> None:
    (tmp_path / "configs").mkdir()
    (tmp_path / "tool").mkdir()
    (tmp_path / "configs" / "verify.yaml").write_text(
        "stages:\n  pytest:\n    roots: [one/tests]\n", encoding="utf-8")
    (tmp_path / "tool" / "verify.yaml").write_text(
        "stages:\n  pytest:\n    roots: [tool/tests]\n"
        "changed:\n- prefix: tool/\n", encoding="utf-8")
    monkeypatch.setattr(runner, "ROOT_DIRECTORY", tmp_path)
    monkeypatch.setattr(runner.config.verify, "project_config_path",
                        Path("configs/verify.yaml"), raising=False)
    monkeypatch.setattr(runner.config.verify, "overlay_config_paths",
                        ["*/verify.yaml"], raising=False)
    loaded = configuration()
    assert loaded["stages"]["pytest"]["roots"] == ["one/tests", "tool/tests"]
    assert loaded["changed"] == [{"prefix": "tool/"}]
