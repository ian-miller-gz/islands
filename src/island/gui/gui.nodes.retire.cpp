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
#include <logger.hpp>
#include <algorithm>
static const String category = "~/island/gui::";

static auto held(GUI::Document &entry) -> Vector<Whole *> {
  auto &pointer = entry.pointer;
  return {&entry.editing,    &pointer.hovered, &pointer.pressed,
          &pointer.armed,    &pointer.wiring,  &pointer.cutting,
          &pointer.banding,  &pointer.snapped, &pointer.panning,
          &pointer.asking,   &pointer.spun,    &pointer.pressing,
          &pointer.stroking, &pointer.lifting};
}

static void release(GUI::Document &entry) {
  for (Whole *const at : ::held(entry))
    if (
      *at != GUI::PARENTLESS &&
      (*at >= entry.nodes.size() || !entry.nodes[*at].live))
      *at = GUI::PARENTLESS;
}

static void reclaim(GUI::Document &entry) {
  while (!entry.nodes.empty() && !entry.nodes.back().live)
    entry.nodes.pop_back();
  std::erase_if(
    entry.dirties, [&entry](Whole at) { return at >= entry.nodes.size(); });
  ::release(entry);
}

auto GUI::NODES::remove(Handle document, STRING::Hot id) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) {
    LOGGER::get(category + "nodes").warn("Unknown node id: " + String(id));
    return 1;
  }
  auto &entry = STATE::documents[document];
  entry.ids.erase(entry.nodes[node].id);
  entry.nodes[node].live = false;
  retire(entry, entry.nodes[node].id.c_str());
  for (Whole index = node + 1; index < entry.nodes.size(); index += 1) {
    auto &below = entry.nodes[index];
    if (!below.live || below.parent == PARENTLESS) continue;
    if (entry.nodes[below.parent].live) continue;
    entry.ids.erase(below.id);
    below.live = false;
    retire(entry, below.id.c_str());
  }
  ::reclaim(entry);
  entry.dirty = true;
  entry.restructured = true;
  return 0;
}
