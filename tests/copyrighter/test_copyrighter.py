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
from pathlib import Path

from copyrighter import Copyrighter


def test_run_writes_headers(tmp_path: Path) -> None:
  python = tmp_path / 'a.py'
  python.write_text('value = 1\n')
  nested = tmp_path / 'sub'
  nested.mkdir()
  cpp = nested / 'b.cpp'
  cpp.write_text('int main() { return 0; }\n')

  Copyrighter().run(tmp_path)

  assert python.read_text().startswith('__copyright__ = """\n')
  assert python.read_text().endswith('value = 1\n')
  assert cpp.read_text().startswith('#pragma region COPYRIGHT\n')
  assert cpp.read_text().endswith('int main() { return 0; }\n')


def test_run_is_idempotent(tmp_path: Path) -> None:
  file = tmp_path / 'a.py'
  file.write_text('value = 1\n')
  copyrighter = Copyrighter()

  copyrighter.run(tmp_path)
  headered = file.read_text()
  stamp = os.stat(file).st_mtime_ns

  copyrighter.run(tmp_path)
  assert file.read_text() == headered
  assert os.stat(file).st_mtime_ns == stamp


def test_run_leaves_unsupported_files_alone(tmp_path: Path) -> None:
  file = tmp_path / 'notes.txt'
  file.write_text('plain text\n')
  Copyrighter().run(tmp_path)
  assert file.read_text() == 'plain text\n'


def test_run_nonrecursive_skips_subdirectories(tmp_path: Path) -> None:
  nested = tmp_path / 'sub'
  nested.mkdir()
  file = nested / 'a.py'
  file.write_text('value = 1\n')
  Copyrighter(recursive=False).run(tmp_path)
  assert file.read_text() == 'value = 1\n'


def test_run_skips_virtualenv_directories(tmp_path: Path) -> None:
  nested = tmp_path / '.venv'
  nested.mkdir()
  file = nested / 'a.py'
  file.write_text('value = 1\n')
  Copyrighter().run(tmp_path)
  assert file.read_text() == 'value = 1\n'
