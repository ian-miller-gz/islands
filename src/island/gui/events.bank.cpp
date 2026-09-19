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

namespace STATE = GUI::STATE;

namespace {

const Whole LIST = GUI::BACKEND::row("list");
const Whole ITEM = GUI::BACKEND::row("item");

}  // namespace

auto GUI::fits(Handle document, Whole node) -> Whole {
  const auto &properties = STATE::documents[document].nodes[node].properties;
  const Float advance = properties.pitch + properties.pad;
  const Float height = BACKEND::measured(document, node).h;
  if (advance <= 0.0f || height <= 0.0f) return 0;
  return static_cast<Whole>((height + properties.pad) / advance);
}

static void settle(GUI::Handle document, Whole index, Whole window) {
  const auto &node = STATE::documents[document].nodes[index];
  const auto &properties = node.properties;
  const Whole tail = properties.rows > window ? properties.rows - window : 0;
  const Whole first = properties.first < tail ? properties.first : tail;
  GUI::set(document, node.id.c_str(), GUI::Scroll{first});
}

static void wear(GUI::Handle document, const String &row, const String &style) {
  if (!style.empty() && STATE::documents[document].styles.contains(style))
    GUI::set(document, row.c_str(), GUI::Style{style.c_str()});
}

static void dress(
  GUI::Handle document, const String &row, const GUI::Properties &list,
  Float width, Flag marked) {
  ::wear(document, row, marked ? list.mark : list.plain);
  GUI::set(document, row.c_str(), GUI::Extent{width, list.pitch});
  if (list.size > 0.0f) GUI::set(document, row.c_str(), GUI::Size{list.size});
  GUI::set(document, row.c_str(), GUI::Visibility{true});
}

static void tend(GUI::Handle document, Whole index) {
  ::settle(document, index, GUI::fits(document, index));
  GUI::conceal(document, index);
  GUI::hoist(document, index);
  const auto id = STATE::documents[document].nodes[index].id;
  const auto list = STATE::documents[document].nodes[index].properties;
  const Whole window = GUI::fits(document, index);
  const Whole shown = list.rows < window ? list.rows : window;
  const Float width = GUI::BACKEND::measured(document, index).w;
  for (Whole at = 0;; at += 1) {
    const String row = String(id) + "." + std::to_string(at);
    Whole seat = GUI::PARENTLESS;
    const Flag exists = GUI::find(document, row.c_str(), seat) == 0;
    if (exists) {
      const auto &node = STATE::documents[document].nodes[seat];
      if (node.row != ::ITEM || node.parent != index) return;
    }
    if (at >= shown) {
      if (!exists) return;
      GUI::set(document, row.c_str(), GUI::Visibility{false});
      continue;
    }
    if (!exists) {
      if (GUI::NODES::create(document, id.c_str(), "item", row.c_str()) != 0)
        return;
      ::wear(document, row, list.plain);
      GUI::compose(document, index, row);
    }
    ::dress(document, row, list, width, list.first + at == list.cursor);
  }
}

void GUI::bank() {
  for (Handle document = 0; document < STATE::documents.size(); document += 1) {
    if (!STATE::documents[document].live) continue;
    const Whole count = STATE::documents[document].nodes.size();
    for (Whole index = 0; index < count; index += 1) {
      const auto &node = STATE::documents[document].nodes[index];
      if (node.live && node.row == ::LIST) ::tend(document, index);
    }
  }
}
