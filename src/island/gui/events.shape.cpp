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
#include <island/window/backend.hpp>

namespace {

const Whole GRIP = GUI::BACKEND::row("grip");

constexpr WINDOW::Shape AXES[] = {WINDOW::DOWN, WINDOW::ACROSS, WINDOW::ARROW};

auto axis(const GUI::Properties &properties) -> Whole {
  return static_cast<Whole>(properties.across) +
         static_cast<Whole>(properties.free);
}

auto wanted(const GUI::Node &node) -> WINDOW::Shape {
  if (node.row == ::GRIP) return ::AXES[::axis(node.properties)];
  return GUI::typeable(node) ? WINDOW::IBEAM : WINDOW::ARROW;
}

auto pulled(const GUI::Document &entry, Whole index) -> Flag {
  return index != GUI::PARENTLESS && entry.nodes[index].live &&
         entry.nodes[index].row == ::GRIP;
}

auto wanted(const GUI::Document &entry) -> WINDOW::Shape {
  const auto pressed = entry.pointer.pressed;
  if (::pulled(entry, pressed)) return ::wanted(entry.nodes[pressed]);
  const auto hovered = entry.pointer.hovered;
  if (hovered == GUI::PARENTLESS || !entry.nodes[hovered].live)
    return WINDOW::ARROW;
  return ::wanted(entry.nodes[hovered]);
}

}  // namespace

void GUI::shape() {
  if (carried()) return;
  WINDOW::Shape asked = WINDOW::ARROW;
  for (const auto &entry : STATE::documents) {
    if (!entry.live || !entry.pointer.placed) continue;
    const auto answer = ::wanted(entry);
    if (answer != WINDOW::ARROW) asked = answer;
  }
  WINDOW::shape(asked);
}
