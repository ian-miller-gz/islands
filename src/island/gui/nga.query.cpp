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
#include <island/gui/nga.hpp>
#include <island/gui/backend.hpp>
#include <island/gui/gui.internal.hpp>

namespace {

const Whole GRAPH = GUI::BACKEND::row("graph");
const Whole NODE = GUI::BACKEND::row("node");

}  // namespace

auto GUI::board(const Document &entry, Whole index) -> Whole {
  for (auto at = entry.nodes[index].parent; at != PARENTLESS;
       at = entry.nodes[at].parent)
    if (entry.nodes[at].row == ::GRAPH) return at;
  return PARENTLESS;
}

auto GUI::board(const Document &entry, const NGA::Link &link) -> Whole {
  const auto found = entry.ids.find(link.from);
  if (found == entry.ids.end()) return PARENTLESS;
  return board(entry, found->second);
}

auto GUI::box(const Document &entry, Whole index) -> Whole {
  for (auto at = index; at != PARENTLESS; at = entry.nodes[at].parent)
    if (entry.nodes[at].row == ::NODE) return at;
  return index;
}

auto GUI::under(const Document &entry, Whole index, Whole root) -> Flag {
  if (root == PARENTLESS) return true;
  for (auto at = index; at != PARENTLESS; at = entry.nodes[at].parent)
    if (at == root) return true;
  return false;
}

auto GUI::NGA::GET::links(Handle document, STRING::Hot id) -> Vector<Link> {
  if (!live(document)) return {};
  const auto &entry = STATE::documents[document];
  if (id[0] == '\0') return entry.links;
  Vector<Link> standing;
  for (const auto &link : entry.links)
    if (link.from == id || link.to == id) standing.push_back(link);
  return standing;
}

auto GUI::NGA::GET::linked(Handle document, STRING::Hot from, STRING::Hot to)
  -> Flag {
  if (!live(document)) return false;
  for (const auto &link : STATE::documents[document].links)
    if (link.from == from && link.to == to) return true;
  return false;
}

auto GUI::NGA::GET::label(Handle document, STRING::Hot from, STRING::Hot to)
  -> STRING::Cold {
  if (!live(document)) return "";
  for (const auto &link : STATE::documents[document].links)
    if (link.from == from && link.to == to) return link.label;
  return "";
}
