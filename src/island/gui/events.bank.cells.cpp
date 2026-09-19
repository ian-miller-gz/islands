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
#include <logger.hpp>
static const String category = "~/island/gui::";

namespace STATE = GUI::STATE;

static auto leaf(const String &id) -> String {
  const auto mark = id.rfind('.');
  return mark == String::npos ? id : id.substr(mark + 1);
}

static auto children(GUI::Handle document, Whole parent) -> Vector<Whole> {
  const auto &entry = STATE::documents[document];
  Vector<Whole> found;
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    if (entry.nodes[index].live && entry.nodes[index].parent == parent)
      found.push_back(index);
  return found;
}

static void graft(GUI::Handle document, Whole cell, const String &host) {
  const auto &entry = STATE::documents[document];
  const String name = host + "." + ::leaf(String(entry.nodes[cell].id));
  const String kind = String(entry.nodes[cell].kind);
  const auto properties = entry.nodes[cell].properties;
  const auto seedlings = ::children(document, cell);
  if (GUI::NODES::create(document, host.c_str(), kind.c_str(), name.c_str()))
    return;
  Whole seat = GUI::PARENTLESS;
  if (GUI::find(document, name.c_str(), seat) != 0) return;
  auto &born = STATE::documents[document].nodes[seat].properties;
  born = properties;
  born.visible = true;
  for (const Whole seedling : seedlings) ::graft(document, seedling, name);
}

static auto pattern(GUI::Handle document, Whole list) -> Whole {
  const auto &properties = STATE::documents[document].nodes[list].properties;
  if (String(properties.pattern).empty()) return GUI::PARENTLESS;
  Whole node = GUI::PARENTLESS;
  if (GUI::find(document, properties.pattern.c_str(), node) == 0) return node;
  LOGGER::get(category + "bank")
    .warn("Unknown row pattern: " + String(properties.pattern));
  return GUI::PARENTLESS;
}

void GUI::conceal(Handle document, Whole list) {
  const Whole node = ::pattern(document, list);
  if (node == PARENTLESS) return;
  const String id = String(STATE::documents[document].nodes[node].id);
  set(document, id.c_str(), Visibility{false});
}

void GUI::compose(Handle document, Whole list, const String &row) {
  const Whole node = ::pattern(document, list);
  if (node == PARENTLESS) return;
  for (const Whole cell : ::children(document, node))
    ::graft(document, cell, row);
}
