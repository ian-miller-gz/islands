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
#include <network/lines.hpp>

namespace NETWORK::LINES {

void Framer::feed(const String &bytes) { held += bytes; }

auto Framer::next(String &line) -> Flag {
  for (;;) {
    auto cut = held.find('\n');
    if (resync) {
      if (cut == String::npos) return held.clear(), false;
      held.erase(0, cut + 1);
      resync = false;
      continue;
    }
    if (cut == String::npos) {
      if (held.size() > LIMIT) ++lost, held.clear(), resync = true;
      return false;
    }
    line = held.substr(0, cut);
    held.erase(0, cut + 1);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    return true;
  }
}

auto Framer::dropped() const -> Whole { return lost; }

}  // namespace NETWORK::LINES
