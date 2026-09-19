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
import ctypes
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]
LIBRARY = ROOT / "build" / "outputs" / "libcgltf.so"
MODELS = ROOT / "tests" / "fixtures" / "models"
BOX = ("meshes 1 primitives 1 nodes 2 buffers 1 bytes 648 "
       "accessors 3 materials 1")


def _summary(path: Path) -> str | None:
  if not LIBRARY.exists():
    pytest.skip(
      "build/outputs/libs/cgltf/libcgltf.so missing; build it: python3 -m make")
  library = ctypes.CDLL(str(LIBRARY))
  library.summary.restype = ctypes.c_char_p
  library.summary.argtypes = [ctypes.c_char_p]
  result = library.summary(str(path).encode())
  return result.decode() if result is not None else None


def test_binary_parses() -> None:
  assert _summary(MODELS / "Box" / "glTF-Binary" / "Box.glb") == BOX


def test_external_buffers_parse() -> None:
  assert _summary(MODELS / "Box" / "glTF" / "Box.gltf") == BOX


def test_lantern_parses() -> None:
  assert _summary(MODELS / "Lantern" / "glTF-Binary" / "Lantern.glb") == (
    "meshes 3 primitives 3 nodes 4 buffers 1 bytes 9560676 "
    "accessors 15 materials 1")


def test_missing_file_fails() -> None:
  assert _summary(MODELS / "missing.gltf") is None
