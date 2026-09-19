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
#include <island/graphics/capture.hpp>
#include <island/window/backend.hpp>
#include <algorithm>
#include <cmath>

namespace {

struct Held {
  Flag on = false;
  GUI::Handle document = GUI::NONE;
  Whole index = GUI::PARENTLESS;
};

Held standing;

auto grid(Float length) -> Whole {
  return static_cast<Whole>(std::lround(length));
}

auto inside(Float where, Whole span) -> Whole {
  const Float far = static_cast<Float>(span) - 1.0f;
  return ::grid(std::clamp(where, 0.0f, std::max(0.0f, far)));
}

struct Plate {
  GUI::Position at;
  GUI::Extent span;
};

auto hanging(const GUI::Document &entry, Whole index, Whole grip) -> Flag {
  for (auto at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent) {
    if (at == grip) return true;
    if (!entry.nodes[at].live || !entry.nodes[at].properties.visible)
      return false;
  }
  return false;
}

auto whole(GUI::Handle document, const GUI::Document &entry, Whole grip)
  -> Plate {
  const auto at = GUI::BACKEND::origin(document, grip);
  const auto span = GUI::BACKEND::measured(document, grip);
  Float west = at.x;
  Float north = at.y;
  Float east = at.x + span.w;
  Float south = at.y + span.h;
  for (Whole index = grip + 1; index < entry.nodes.size(); ++index) {
    if (!::hanging(entry, index, grip)) continue;
    const auto corner = GUI::BACKEND::origin(document, index);
    const auto extent = GUI::BACKEND::measured(document, index);
    west = std::min(west, corner.x);
    north = std::min(north, corner.y);
    east = std::max(east, corner.x + extent.w);
    south = std::max(south, corner.y + extent.h);
  }
  return {{west, north}, {east - west, south - north}};
}

}  // namespace

void GUI::carry(Handle document, Document &entry, Whole index, Flag on) {
  const auto id = entry.nodes[index].id.c_str();
  if (!on) {
    if (
      !standing.on || standing.document != document || standing.index != index)
      return;
    set(document, id, Visibility{true});
    standing = {};
    return;
  }
  if (!entry.pointer.placed || !entry.nodes[index].properties.carried) return;
  const auto &rect = entry.pointer.placement;
  const auto extent = BACKEND::extent(document);
  if (rect.w <= 0.0f || rect.h <= 0.0f || extent.w <= 0.0f || extent.h <= 0.0f)
    return;
  const Float across = rect.w / extent.w;
  const Float down = rect.h / extent.h;
  const auto plate = ::whole(document, entry, index);
  const Float x = rect.x + plate.at.x * across;
  const Float y = rect.y + plate.at.y * down;
  if (x < 0.0f || y < 0.0f || plate.span.w <= 0.0f || plate.span.h <= 0.0f)
    return;
  const auto picture = GFX::CAPTURE::read(
    ::grid(x), ::grid(y), ::grid(plate.span.w * across),
    ::grid(plate.span.h * down));
  if (picture.w == 0 || picture.h == 0) return;
  WINDOW::wear(
    picture, ::inside((entry.pointer.grab.x - plate.at.x) * across, picture.w),
    ::inside((entry.pointer.grab.y - plate.at.y) * down, picture.h));
  if (!WINDOW::GET::worn()) return;
  set(document, id, Visibility{false});
  standing = {true, document, index};
}

auto GUI::carried() -> Flag {
  if (!standing.on) return false;
  if (live(standing.document)) {
    const auto &entry = STATE::documents[standing.document];
    if (standing.index < entry.nodes.size() && entry.nodes[standing.index].live)
      return true;
  }
  standing = {};
  return false;
}
