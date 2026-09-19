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
import make.config as CONFIG
import make.initializer as initializer_module
from make import api
from make.initializer import Initializer


def test_compile_head_shape() -> None:
  head = api.compile_head()
  assert head[0] == CONFIG.COMPILER
  assert head[1:1 + len(CONFIG.STANDARD)] == CONFIG.STANDARD
  assert "-isystem" in head
  assert any(part.startswith("-I") for part in head)
  assert "-fsyntax-only" not in head and "-E" not in head


def test_preprocess_head_appends_the_preprocess_mode() -> None:
  head = api.preprocess_head()
  assert head[:-3] == api.compile_head()
  assert head[-3:] == ["-E", "-x", "c++"]


def test_survey_matches_initialize() -> None:
  surveyed = Initializer()
  surveyed.survey()
  initialized = Initializer()
  initialized.initialize()
  assert surveyed.flags == initialized.flags
  assert surveyed.roots == initialized.roots
  assert surveyed.include_paths == initialized.include_paths


def test_survey_is_read_only(tmp_path, monkeypatch) -> None:
  (tmp_path / "src" / "widget").mkdir(parents=True)
  (tmp_path / "src" / "widget" / "widget.hpp").write_text("#pragma once\n")
  monkeypatch.setattr(initializer_module, "ROOT_DIRECTORY", tmp_path)
  surveyed = Initializer()
  surveyed._project_cfg = {}
  surveyed.survey()
  assert tmp_path / "src" / "widget" in surveyed.roots
  assert not (tmp_path / "build").exists()
  assert not (tmp_path / "logs").exists()
