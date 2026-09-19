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
#include <logger.hpp>
#include <test/gui/gui.internal.hpp>

namespace {

constexpr STRING::Hot NAME = "baked";

}  // namespace

Status baked(STRING::Hot path) {
  LOGGER::initialize();
  GUI::bind("icons", 0);
  Vector<Byte> bytes;
  if (IO::read(path, bytes) != 0) return 1;
  const auto memory = GUI::load(::NAME, bytes);
  if (memory == GUI::NONE) return 1;
  const auto original = GUI::load(path);
  Status status = check("file route parses", original != GUI::NONE);
  status |=
    check("baked tree matches the file's", lines(memory) == lines(original));
  status |= check(
    "baked name collision refuses", GUI::load(::NAME, bytes) == GUI::NONE);
  status |= check("path collision refuses", GUI::load(path) == GUI::NONE);
  return status;
}
