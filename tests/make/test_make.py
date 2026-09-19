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

from make.config import (
    BUILD_CACHE_NAME,
    BUILD_OBJECT_PATH,
    BUILD_PATH,
    LOG_PATH,
)
from make.make import Make


def _bare_make(tmp_path) -> Make:
    make = Make.__new__(Make)
    make.root = tmp_path
    make.mirror = BUILD_OBJECT_PATH
    make.cache = {'a.cpp': [1.0, 'old']}
    make.metadata = {'a.cpp': 2.0}
    make.stamp = 3.0
    make.prints = {'a.cpp': 'print'}
    make.output = {
        'compile': {'out': b'', 'err': b''},
        'libraries': {},
        'builds': {},
    }
    os.makedirs(tmp_path / LOG_PATH, exist_ok=True)
    os.makedirs(tmp_path / BUILD_PATH, exist_ok=True)
    os.makedirs(tmp_path / BUILD_OBJECT_PATH, exist_ok=True)
    return make


def test_make_has_methods() -> None:
    assert hasattr(Make, 'compile')
    assert hasattr(Make, 'link')
    assert callable(getattr(Make, 'compile'))
    assert callable(getattr(Make, 'link'))


def test_clear_updates_cache_on_success(tmp_path) -> None:
    make = _bare_make(tmp_path)
    make.errored = False
    make.clear()
    assert make.cache == {'a.cpp': [3.0, 'print']}
    assert (tmp_path / BUILD_OBJECT_PATH / BUILD_CACHE_NAME).exists()


def test_clear_keeps_cache_when_errored(tmp_path) -> None:
    make = _bare_make(tmp_path)
    make.errored = True
    make.clear()
    assert make.cache == {'a.cpp': [1.0, 'old']}
    assert not (tmp_path / BUILD_OBJECT_PATH / BUILD_CACHE_NAME).exists()
