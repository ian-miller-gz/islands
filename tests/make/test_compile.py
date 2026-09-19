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
from types import SimpleNamespace
from make.compiler import Compiler
from make.config import COMPILER
from env import Path


def test_build_command_and_write_logs(tmp_path) -> None:
    make: SimpleNamespace = SimpleNamespace()
    make.roots = [tmp_path / 'include']
    make.include_paths = {'include'}
    make.root = tmp_path
    make.output = {'compile': {'out': b'', 'err': b''}}
    make.errored = False

    compiler = Compiler(make)
    cmd: list = compiler._build_command(Path('src/main.cpp'), Path('build/wip.main.o'))
    assert isinstance(cmd, list)
    assert COMPILER in cmd
    assert '-o' in cmd

    compiler._write_compile_logs(b'outbytes', b'errbytes')
    assert b'outbytes' in make.output['compile']['out']
    assert b'errbytes' in make.output['compile']['err']
