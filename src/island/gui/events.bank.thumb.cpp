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
#include <island/gui/events.internal.hpp>
#include <island/gui/backend.hpp>
#include <algorithm>
#include <cmath>

namespace STATE = GUI::STATE;

static constexpr Float SPAN = 2.0f;

void GUI::hoist(Handle document, Whole index) {
  const auto id = STATE::documents[document].nodes[index].id;
  const String mark = String(id) + ".scroll";
  const auto list = STATE::documents[document].nodes[index].properties;
  const Whole window = fits(document, index);
  const Flag wanted = list.scroll && window > 0 && list.rows > window;
  Whole seat = PARENTLESS;
  if (find(document, mark.c_str(), seat) == 0) {
    if (
      STATE::documents[document].nodes[seat].kind != "panel" ||
      STATE::documents[document].nodes[seat].parent != index)
      return;
  } else {
    if (!wanted) return;
    if (NODES::create(document, id.c_str(), "panel", mark.c_str()) != 0) return;
    if (find(document, mark.c_str(), seat) != 0) return;
    set(document, mark.c_str(), NORTHEAST);
    set(document, mark.c_str(), Depth{1.0f});
    set(document, mark.c_str(), Border{0.0f});
  }
  STATE::documents[document].nodes[seat].rim = true;
  set(document, mark.c_str(), Visibility{wanted});
  if (!wanted) return;
  const Float height = BACKEND::measured(document, index).h;
  const Float rows = static_cast<Float>(list.rows);
  const Float span =
    std::max(SPAN, std::round(height * static_cast<Float>(window) / rows));
  const Float room = std::max(0.0f, height - span);
  const Float down = std::round(height * static_cast<Float>(list.first) / rows);
  set(document, mark.c_str(), Extent{SPAN, span});
  set(document, mark.c_str(), Position{0.0f, std::min(down, room)});
  set(document, mark.c_str(), list.edge);
}
