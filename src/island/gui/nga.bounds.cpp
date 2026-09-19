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
#include <island/gui/nga.internal.hpp>
#include <island/gui/backend.hpp>
#include <algorithm>

namespace {

auto within(Float pan, Float low, Float high, Float window) -> Float {
  Float settled = pan;
  if (high != GUI::NGA::Bounds::NONE)
    settled = std::min(settled, high - window);
  if (low != GUI::NGA::Bounds::NONE) settled = std::max(settled, low);
  return settled;
}

}  // namespace

auto GUI::NGA::bounded(Handle document, Whole index, Pan pan) -> Pan {
  const auto &worn = STATE::documents[document].nodes[index].properties;
  const auto size = BACKEND::measured(document, index);
  const Float across = size.w / (worn.zoom.w > 0.0f ? worn.zoom.w : 1.0f);
  const Float down = size.h / (worn.zoom.h > 0.0f ? worn.zoom.h : 1.0f);
  return {
    ::within(pan.x, worn.walls.west, worn.walls.east, across),
    ::within(pan.y, worn.walls.north, worn.walls.south, down)};
}

void GUI::NGA::settle(Handle document, Whole index) {
  auto &properties = STATE::documents[document].nodes[index].properties;
  const auto settled =
    bounded(document, index, {properties.pan.x, properties.pan.y});
  properties.pan = {settled.x, settled.y};
}

auto GUI::NGA::set(Handle document, STRING::Hot id, Bounds bounds) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.walls == bounds) return 0;
  properties.walls = bounds;
  const auto settled =
    bounded(document, node, {properties.pan.x, properties.pan.y});
  properties.pan = {settled.x, settled.y};
  return mark(document, node);
}

auto GUI::NGA::GET::bounds(Handle document, STRING::Hot id) -> Bounds {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.walls;
}
