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
from verify import diagnostics


def test_weights_passes_within_budget(capsys) -> None:
  table = [{"header": "src/common/types.hpp", "lines": 10_000_000}]
  assert diagnostics.weights(table)
  assert "weights  PASS" in capsys.readouterr().out


def test_weights_fails_over_budget(capsys) -> None:
  table = [{"header": "src/common/types.hpp", "lines": 1}]
  assert not diagnostics.weights(table)
  out = capsys.readouterr().out
  assert "weights  FAIL" in out and "budget 1" in out


def test_weights_fails_on_a_header_that_cannot_preprocess(capsys) -> None:
  table = [{"header": "src/does/not/exist.hpp", "lines": 1000}]
  assert not diagnostics.weights(table)
  assert "failed to preprocess" in capsys.readouterr().out


def test_empty_table_passes(capsys) -> None:
  assert diagnostics.weights([])
  assert "weights  PASS" in capsys.readouterr().out
