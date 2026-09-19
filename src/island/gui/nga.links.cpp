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
#include <island/gui/gui.internal.hpp>

namespace {

auto joinable(GUI::Handle document, STRING::Hot from, STRING::Hot to) -> Flag {
  Whole source = GUI::PARENTLESS;
  Whole sink = GUI::PARENTLESS;
  if (GUI::find(document, from, source) != 0) return false;
  if (GUI::find(document, to, sink) != 0) return false;
  if (source == sink) return false;
  const auto &entry = GUI::STATE::documents[document];
  const auto deck = GUI::board(entry, source);
  if (deck == GUI::PARENTLESS || deck != GUI::board(entry, sink)) return false;
  return GUI::NGA::agree(entry, source, sink);
}

auto placed(GUI::Document &entry, STRING::Hot from, STRING::Hot to) -> Whole {
  for (Whole at = 0; at < entry.links.size(); at += 1)
    if (entry.links[at].from == from && entry.links[at].to == to) return at;
  return static_cast<Whole>(entry.links.size());
}

}  // namespace

auto GUI::NGA::agree(const Document &entry, Whole source, Whole sink) -> Flag {
  const auto &out = entry.nodes[source].properties.kind;
  const auto &in = entry.nodes[sink].properties.kind;
  return out.empty() || in.empty() || out == in;
}

auto GUI::NGA::connect(Handle document, STRING::Hot from, STRING::Hot to)
  -> Status {
  return connect(document, from, to, Label{});
}

auto GUI::NGA::connect(
  Handle document, STRING::Hot from, STRING::Hot to, Label label) -> Status {
  if (!live(document) || !::joinable(document, from, to)) return 1;
  auto &entry = STATE::documents[document];
  const auto at = ::placed(entry, from, to);
  if (at < entry.links.size()) {
    if (entry.links[at].label == label.value) return 0;
    entry.links[at].label = label.value;
    entry.dirty = true;
    return 0;
  }
  entry.links.push_back({from, to, label.value});
  entry.dirty = true;
  entry.restructured = true;
  return 0;
}

auto GUI::NGA::disconnect(Handle document, STRING::Hot from, STRING::Hot to)
  -> Status {
  if (!live(document)) return 1;
  auto &entry = STATE::documents[document];
  const auto at = ::placed(entry, from, to);
  if (at == entry.links.size()) return 1;
  entry.links.erase(entry.links.begin() + at);
  entry.dirty = true;
  entry.restructured = true;
  return 0;
}

auto GUI::NGA::connect(
  Handle document, STRING::Hot from, STRING::Hot to, Style style) -> Status {
  if (!live(document) || !::joinable(document, from, to)) return 1;
  auto &entry = STATE::documents[document];
  const auto at = ::placed(entry, from, to);
  if (at < entry.links.size()) {
    if (entry.links[at].style == style.name) return 0;
    entry.links[at].style = style.name;
    entry.dirty = true;
    return 0;
  }
  entry.links.push_back({from, to, "", style.name});
  entry.dirty = true;
  entry.restructured = true;
  return 0;
}
