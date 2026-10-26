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
#include <algorithm>

namespace {

using namespace INPUT::KEYS;

constexpr STRING::Hot KINDS[] = {"field", "dial", "button"};

constexpr Action HANDED[] = {UP, DOWN, PRIOR, NEXT};

auto shown(const GUI::Document &entry, Whole index) -> Flag {
  for (Whole at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent)
    if (!entry.nodes[at].live || !entry.nodes[at].properties.visible)
      return false;
  return true;
}

auto click(
  GUI::Handle document, GUI::Document &entry, Whole index,
  const Event &event) -> Flag {
  if (event.action == ENTER) {
    GUI::record(entry, index, GUI::Event::CLICKED);
    entry.pointer.clicks.push_back(entry.nodes[index].id);
    return true;
  }
  if (std::ranges::find(::HANDED, event.action) != std::end(::HANDED))
    GUI::seat(document, GUI::PARENTLESS, false);
  return false;
}

}  // namespace

auto GUI::reachable(const Node &node) -> Flag {
  return std::find(std::begin(::KINDS), std::end(::KINDS), node.kind) !=
         std::end(::KINDS);
}

auto GUI::ring(const Document &entry, Whole index) -> Whole {
  const auto count = entry.nodes.size();
  for (Whole step = 1; step <= count; step += 1) {
    const auto at = (index + step) % count;
    if (::shown(entry, at) && reachable(entry.nodes[at])) return at;
  }
  return index;
}

auto GUI::reach(Handle document, Whole index, const INPUT::KEYS::Event &event)
  -> Flag {
  auto &entry = STATE::documents[document];
  if (!::shown(entry, index)) {
    seat(document, PARENTLESS, false);
    return false;
  }
  if (
    event.action == INPUT::KEYS::TEXT && event.codepoint == INPUT::KEYS::TAB) {
    const auto next = ring(entry, index);
    seat(document, next, entry.nodes[next].kind == "field");
    return true;
  }
  if (entry.nodes[index].kind == "dial") return nudge(document, index, event);
  return ::click(document, entry, index, event);
}

auto GUI::shut(
  Handle document, Document &entry, const INPUT::KEYS::Event &event) -> Flag {
  if (event.action != INPUT::KEYS::TEXT) return false;
  if (event.codepoint != INPUT::KEYS::ESCAPE) return false;
  const auto cell = entry.editing;
  if (cell == PARENTLESS || !entry.seated || !entry.nodes[cell].live)
    return true;
  if (entry.nodes[cell].kind == "dial") seat(document, cell, false);
  return true;
}
