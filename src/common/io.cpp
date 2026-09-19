#pragma region COPYRIGHT
/* ===========================================================================
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
 ============================================================================*/
#pragma endregion
#include <common/io.hpp>

auto IO::read(const String &path, Vector<Byte> &bytes) -> Status {
  bytes.clear();
  STREAMS::Input stream(path, std::ios::binary);
  if (!stream) return 1;
  bytes.assign(
    std::istreambuf_iterator<Char>(stream), std::istreambuf_iterator<Char>());
  return 0;
}

auto IO::read(const String &path) -> Vector<Byte> {
  Vector<Byte> bytes;
  read(path, bytes);
  return bytes;
}

auto IO::sibling(const String &anchor, const String &name) -> String {
  const auto slash = anchor.find_last_of('/');
  return slash == String::npos ? name : anchor.substr(0, slash + 1) + name;
}
