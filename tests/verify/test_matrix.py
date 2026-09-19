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
from verify.matrix import cells, matrix

CURATED = ["sdl", "static-meadow", "release", "bench"]


def test_explicit_names_run_verbatim() -> None:
  assert cells(["release"], CURATED, None) == ["release"]


def test_default_is_the_curated_list() -> None:
  assert cells([], CURATED, None) == CURATED


def test_changed_narrows_to_named_cells_in_curated_order() -> None:
  narrowed = {"pytest": [], "golds": ["pixels"], "matrix": ["bench", "sdl"]}
  assert cells([], CURATED, narrowed) == ["sdl", "bench"]


def test_changed_naming_no_cells_runs_none() -> None:
  narrowed = {"pytest": ["tests/make"], "golds": [], "matrix": []}
  assert cells([], CURATED, narrowed) == []


def test_explicit_names_override_narrowing() -> None:
  narrowed = {"pytest": [], "golds": [], "matrix": []}
  assert cells(["sdl"], CURATED, narrowed) == ["sdl"]


def test_cell_gold_selection_runs_exactly_the_listed_filters(capsys) -> None:
  ran: list[list[str]] = []
  from verify import runner

  original = runner.run
  runner.run = lambda name, command, directory: ran.append(command) or True
  try:
    configuration = {
      "stages": {
        "build": {"command": ["make"], "directory": ".",
                  "configuration": ["--configuration"]},
        "golds": {"command": ["autest"], "directory": "tests",
                  "filters": ["-f"]}},
      "matrix": ["web-wasm"],
      "cells": {"web-wasm": {"golds": ["page", "pure"]}}}
    assert matrix([], None, configuration) == 0
  finally:
    runner.run = original
  assert ran == [["make", "--configuration", "web-wasm"],
                 ["autest", "-f", "page", "pure"]]


def test_cell_gold_selection_ignores_a_changed_narrowing(capsys) -> None:
  ran: list[list[str]] = []
  from verify import runner

  original = runner.run
  runner.run = lambda name, command, directory: ran.append(command) or True
  try:
    configuration = {
      "stages": {
        "build": {"command": ["make"], "directory": ".",
                  "configuration": ["--configuration"]},
        "golds": {"command": ["autest"], "directory": "tests",
                  "filters": ["-f"]}},
      "matrix": ["web-wasm"],
      "cells": {"web-wasm": {"golds": ["page"]}}}
    narrowed = {"pytest": [], "golds": ["pixels"], "matrix": ["web-wasm"]}
    assert matrix([], narrowed, configuration) == 0
  finally:
    runner.run = original
  assert ran[1] == ["autest", "-f", "page"]


def test_an_empty_cell_gold_selection_runs_no_golds(capsys) -> None:
  ran: list[list[str]] = []
  from verify import runner

  original = runner.run
  runner.run = lambda name, command, directory: ran.append(command) or True
  try:
    configuration = {
      "stages": {
        "build": {"command": ["make"], "directory": ".",
                  "configuration": ["--configuration"]},
        "golds": {"command": ["autest"], "directory": "tests",
                  "filters": ["-f"]}},
      "matrix": ["windows"],
      "cells": {"windows": {"golds": []}}}
    assert matrix([], None, configuration) == 0
  finally:
    runner.run = original
  assert ran == [["make", "--configuration", "windows"]]
  assert "windows:golds SKIP" in capsys.readouterr().out


def test_project_cells_name_declared_configurations_and_tests() -> None:
  import yaml

  from verify.runner import configuration
  from env import ROOT_DIRECTORY, config

  entries = configuration().get("cells") or {}
  with open(ROOT_DIRECTORY / config.make.project_config_path) as file:
    declared = (yaml.safe_load(file) or {}).get("configurations") or {}
  for cell, entry in entries.items():
    assert cell in declared
    for name in entry.get("golds") or []:
      assert (ROOT_DIRECTORY / "tests" / f"{name}.test.py").exists()


def test_matrix_refuses_a_build_stage_without_the_overlay_flag(capsys) -> None:
  configuration = {
    "stages": {"build": {"command": ["true"], "directory": "."},
               "golds": {"command": ["true"], "directory": "."}},
    "matrix": ["cell"]}
  assert matrix([], None, configuration) == 1
  assert "`configuration:` key" in capsys.readouterr().out


def test_project_stages_state_the_runner_flag_spellings() -> None:
  from verify.runner import configuration

  stages = configuration().get("stages") or {}
  assert stages["build"]["configuration"] == ["--configuration"]
  assert stages["golds"]["filters"] == ["-f"]


def test_project_matrix_names_declared_configurations() -> None:
  import yaml

  from verify.runner import configuration
  from env import ROOT_DIRECTORY, config

  curated = configuration().get("matrix") or []
  assert len(curated) >= 3
  with open(ROOT_DIRECTORY / config.make.project_config_path) as file:
    declared = (yaml.safe_load(file) or {}).get("configurations") or {}
  assert set(curated) <= set(declared)
