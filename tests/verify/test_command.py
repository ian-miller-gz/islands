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
import pytest

import make.config as CONFIG
from env import ROOT_DIRECTORY
from make import api
from verify import diagnostics


def test_command_prints_the_compile_head(capsys):
  assert diagnostics.head() == 0
  parts = capsys.readouterr().out.splitlines()
  assert parts[0] == CONFIG.COMPILER
  assert "-isystem" in parts
  assert "-fsyntax-only" not in parts


def test_unknown_configuration_is_refused(capsys):
  with pytest.raises(LookupError):
    api.compile_head("no-such-cell")
  assert diagnostics.syntax("src/common/string.cpp", "no-such-cell") == 1
  assert "verify: syntax FAIL" in capsys.readouterr().out
