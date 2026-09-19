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
#include <island/graphics/sprites/sprites.descriptor.internal.hpp>

namespace DESCRIPTOR = GFX::SPRITES::DESCRIPTOR;

auto DESCRIPTOR::number(const String &text, Whole &answer) -> Status {
  if (text.empty()) return MALFORMED;
  answer = 0;
  for (const Char digit : text) {
    if (digit < '0' || digit > '9') return MALFORMED;
    answer = answer * 10 + static_cast<Whole>(digit - '0');
  }
  return 0;
}

static auto pair(const String &text, Whole &first, Whole &second) -> Status {
  const auto comma = text.find(',');
  if (comma == String::npos) return DESCRIPTOR::MALFORMED;
  if (DESCRIPTOR::number(text.substr(0, comma), first) != 0)
    return DESCRIPTOR::MALFORMED;
  return DESCRIPTOR::number(text.substr(comma + 1), second);
}

auto DESCRIPTOR::apply(const String &key, const String &value, Cut &cut)
  -> Status {
  auto &placement = cut.placement;
  if (key == "x") return number(value, cut.origin.x);
  if (key == "y") return number(value, cut.origin.y);
  if (key == "w") return number(value, placement.extent.width);
  if (key == "h") return number(value, placement.extent.height);
  if (key == "trim") return ::pair(value, placement.trim.x, placement.trim.y);
  if (key == "pivot")
    return ::pair(value, placement.pivot.x, placement.pivot.y);
  if (key != "source") return UNKNOWN;
  cut.sourced = true;
  return ::pair(value, placement.source.width, placement.source.height);
}
