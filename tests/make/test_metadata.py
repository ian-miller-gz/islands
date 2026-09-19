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
from pathlib import Path as PyPath
from env import Path
from make.metadata import Metadata


def test_metadata_reads_timestamp(tmp_path: PyPath) -> None:
    tmp_file = tmp_path / "test.cpp"
    tmp_file.write_text("int main(){}")
    path: Path = Path(str(tmp_file))
    m: Metadata = Metadata(path)
    assert m.path == path
    assert isinstance(m.timestamp, float)
    assert m.timestamp > 0
