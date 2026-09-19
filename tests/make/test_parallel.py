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

import yaml

from make.config import BUILD_CACHE_NAME, PROJECT_CONFIG_PATH
from make.make import Make


COUNT = 10
BROKEN = 5


def _configure(tmp_path, workers: int | None = None) -> None:
  config: dict = {"tokens": {"values": {"NONE": 1}}}
  if workers is not None:
    config["workers"] = workers
  path = tmp_path / PROJECT_CONFIG_PATH
  path.parent.mkdir(parents=True, exist_ok=True)
  with open(path, "w", encoding="utf-8") as file:
    yaml.safe_dump(config, file)


def _sources(tmp_path) -> None:
  source = tmp_path / "src"
  source.mkdir(exist_ok=True)
  for index in range(COUNT):
    (source / f"unit{index:02d}.cpp").write_text(
      f"int unit{index:02d}() {{ return {index}; }}\n")


def _break(tmp_path) -> None:
  (tmp_path / "src" / f"unit{BROKEN:02d}.cpp").write_text(
    "int broken( {\n")


def _pending(metadata) -> int:
  count = 0
  for key, value in metadata.items():
    if isinstance(value, dict):
      count += _pending(value)
    elif key.endswith(".cpp"):
      count += 1
  return count


def _build(monkeypatch, tmp_path) -> tuple[Make, int]:
  monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
  make = Make()
  make.preprocess()
  pending = _pending(make.metadata)
  make.compile()
  make.clear()
  return make, pending


def test_failure_mid_list_errors_and_skips_the_cache(
  monkeypatch, tmp_path, caplog
) -> None:
  _configure(tmp_path)
  _sources(tmp_path)
  _break(tmp_path)
  make, pending = _build(monkeypatch, tmp_path)
  assert make.errored and pending == COUNT
  assert f"unit{BROKEN:02d}.cpp" in caplog.text
  cache = tmp_path / "build" / "objects" / make.zone / BUILD_CACHE_NAME
  assert not cache.exists()


def test_retry_compiles_everything_then_nothing(monkeypatch, tmp_path) -> None:
  _configure(tmp_path)
  _sources(tmp_path)
  _break(tmp_path)
  make, _ = _build(monkeypatch, tmp_path)
  assert make.errored
  _sources(tmp_path)
  make, retried = _build(monkeypatch, tmp_path)
  assert not make.errored and retried == COUNT
  objects = tmp_path / "build" / "objects" / make.zone / "src"
  assert len(list(objects.glob("wip.*.o"))) == COUNT
  make, rebuilt = _build(monkeypatch, tmp_path)
  assert not make.errored and rebuilt == 0


def test_width_reads_the_config_and_defaults_to_the_cpus(
  monkeypatch, tmp_path
) -> None:
  _sources(tmp_path)
  _configure(tmp_path, workers=2)
  monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", tmp_path)
  assert Make().compiler._width() == 2
  _configure(tmp_path)
  assert Make().compiler._width() == (os.cpu_count() or 1)
