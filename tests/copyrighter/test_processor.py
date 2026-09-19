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

import pytest

from copyrighter.data import HEADER, IDENTIFIER
from copyrighter.processor import Processor


PYTHON_BLOCK = f'__copyright__ = """\n{HEADER}"""\n'
CPP_BLOCK = f'#pragma region COPYRIGHT\n/*{HEADER}*/\n#pragma endregion\n'
CPP_LINE = f'// {IDENTIFIER}\n'
PYTHON_LINE = f'# {IDENTIFIER}\n'


@pytest.fixture
def bundle(tmp_path: Path):
  directory = tmp_path / 'cartridges' / 'sample'
  directory.mkdir(parents=True)
  return directory


def scoped(bundle: Path) -> Processor:
  return Processor(root=bundle.parent.parent)


def test_python_header_inserted(tmp_path: Path) -> None:
  file = tmp_path / 'sample.py'
  original = "print('hello')\n"
  file.write_text(original)
  output = Processor().process(file)
  assert output.startswith(PYTHON_BLOCK)
  assert output.endswith(original)


def test_cpp_header_inserted(tmp_path: Path) -> None:
  file = tmp_path / 'sample.cpp'
  original = 'int main() { return 0; }\n'
  file.write_text(original)
  output = Processor().process(file)
  assert output.startswith(CPP_BLOCK)
  assert output.endswith(original)


def test_python_stale_header_replaced(tmp_path: Path) -> None:
  file = tmp_path / 'stale.py'
  file.write_text('__copyright__ = """\nOLD HEADER\n"""\n\nvalue = 1\n')
  output = Processor().process(file)
  assert 'OLD HEADER' not in output
  assert output.count('__copyright__') == 1
  assert output.startswith(PYTHON_BLOCK)
  assert output.endswith('value = 1\n')


def test_cpp_stale_header_replaced(tmp_path: Path) -> None:
  file = tmp_path / 'stale.cpp'
  stale = '#pragma region COPYRIGHT\n/*\nOLD HEADER\n*/\n#pragma endregion\n\n'
  original = 'int main() { return 0; }\n'
  file.write_text(stale + stale + original)
  output = Processor().process(file)
  assert 'OLD HEADER' not in output
  assert output.count('#pragma region COPYRIGHT') == 1
  assert output.startswith(CPP_BLOCK)
  assert output.endswith(original)


def test_python_future_import_stays_first(tmp_path: Path) -> None:
  file = tmp_path / 'future.py'
  file.write_text('from __future__ import annotations\n\nvalue = 1\n')
  output = Processor().process(file)
  assert output.startswith('from __future__ import annotations\n')
  assert PYTHON_BLOCK in output
  assert output.index(PYTHON_BLOCK) > 0
  assert output.endswith('value = 1\n')


def test_process_is_idempotent(tmp_path: Path) -> None:
  cases = {
    'plain.py': 'value = 1\n',
    'future.py': 'from __future__ import annotations\n\nvalue = 1\n',
    'plain.cpp': 'int main() { return 0; }\n',
  }
  processor = Processor()
  for name, original in cases.items():
    file = tmp_path / name
    file.write_text(original)
    once = processor.process(file)
    file.write_text(once)
    twice = processor.process(file)
    assert once == twice, name


def test_cartridge_sources_take_the_identifier(bundle: Path) -> None:
  cpp = bundle / 'sample.cpp'
  cpp.write_text(original := 'int main() { return 0; }\n')
  assert scoped(bundle).process(cpp) == CPP_LINE + original
  python = bundle / 'sample.py'
  python.write_text(script := 'value = 1\n')
  assert scoped(bundle).process(python) == PYTHON_LINE + script


def test_cartridge_block_converts_to_the_identifier(bundle: Path) -> None:
  file = bundle / 'stale.cpp'
  file.write_text(CPP_BLOCK + '\nint main() { return 0; }\n')
  output = scoped(bundle).process(file)
  assert output == CPP_LINE + 'int main() { return 0; }\n'


def test_identifier_is_idempotent(bundle: Path) -> None:
  processor = scoped(bundle)
  for name, original in {'a.cpp': 'int value = 1;\n', 'a.py': 'value = 1\n'}.items():
    file = bundle / name
    file.write_text(original)
    once = processor.process(file)
    file.write_text(once)
    assert processor.process(file) == once, name


def test_unsupported_extension_skipped(tmp_path: Path) -> None:
  file = tmp_path / 'notes.txt'
  file.write_text('plain text\n')
  assert Processor().process(file) == ''
