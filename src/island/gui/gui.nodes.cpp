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
#include <island/gui/gui.internal.hpp>
#include <island/gui/backend.hpp>
#include <logger.hpp>
static const String category = "~/island/gui::";

static auto locate(GUI::Handle document, STRING::Hot parent, Whole &index)
  -> Status {
  index = GUI::PARENTLESS;
  if (*parent == '\0') return 0;
  return GUI::find(document, parent, index);
}

auto GUI::NODES::create(
  Handle document, STRING::Hot parent, STRING::Hot kind,
  STRING::Hot id) -> Status {
  auto &logger = LOGGER::get(category + "nodes");
  if (!live(document)) return 1;
  auto &entry = STATE::documents[document];
  const String name(id);
  if (name.empty() || entry.ids.contains(name)) {
    logger.warn("Node id refused: '" + name + "'");
    return 1;
  }
  Whole index = PARENTLESS;
  if (::locate(document, parent, index) != 0) {
    logger.warn("Unknown parent id: " + String(parent));
    return 1;
  }
  entry.nodes.push_back(
    {kind, BACKEND::row(kind), name, index, BACKEND::defaults(kind), true,
     true});
  entry.ids[name] = entry.nodes.size() - 1;
  entry.dirty = true;
  entry.restructured = true;
  return 0;
}

auto GUI::GET::nodes(Handle document) -> Pool {
  if (!live(document)) return {};
  const auto &nodes = STATE::documents[document].nodes;
  Pool pool;
  for (const auto &node : nodes) (node.live ? pool.live : pool.dead) += 1;
  return pool;
}
