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
#include <island/input.hpp>
#include <algorithm>
#include <cstdlib>

namespace STATE = GUI::STATE;

namespace {

const Whole LIST = GUI::BACKEND::row("list");
const Whole ITEM = GUI::BACKEND::row("item");

}  // namespace

auto GUI::focus(Handle document, Flag on) -> Status {
  if (!live(document)) return 1;
  if (on) {
    STATE::focused = document;
    INPUT::TEXT::focus(true);
    return 0;
  }
  if (STATE::focused == document) {
    STATE::focused = NONE;
    INPUT::TEXT::focus(false);
  }
  return 0;
}

static auto shown(const GUI::Document &entry, Whole index) -> Flag {
  for (Whole at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent)
    if (!entry.nodes[at].live || !entry.nodes[at].properties.visible)
      return false;
  return true;
}

static auto standing(const GUI::Document &entry, Whole index) -> Float {
  Float z = 0.0f;
  for (Whole at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent)
    z = std::max(z, entry.nodes[at].properties.z);
  return z;
}

static auto steered(const GUI::Document &entry) -> Whole {
  Whole found = GUI::PARENTLESS;
  Float raised = 0.0f;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    const auto &node = entry.nodes[index];
    if (!node.live || node.row != ::LIST || !::shown(entry, index)) continue;
    const Float z = ::standing(entry, index);
    if (found != GUI::PARENTLESS && z <= raised) continue;
    found = index;
    raised = z;
  }
  return found;
}

static auto steers(INPUT::KEYS::Action action) -> Flag {
  constexpr INPUT::KEYS::Action WALKS[] = {
    INPUT::KEYS::UP,  INPUT::KEYS::DOWN,  INPUT::KEYS::HOME,
    INPUT::KEYS::END, INPUT::KEYS::PRIOR, INPUT::KEYS::NEXT};
  for (const auto walk : WALKS)
    if (action == walk) return true;
  return false;
}

static auto landed(
  const INPUT::KEYS::Event &event, Whole cursor, Whole rows,
  Whole page) -> Whole {
  if (event.action == INPUT::KEYS::UP) return (cursor + rows - 1) % rows;
  if (event.action == INPUT::KEYS::DOWN) return (cursor + 1) % rows;
  if (event.action == INPUT::KEYS::HOME) return 0;
  if (event.action == INPUT::KEYS::END) return rows - 1;
  if (event.action == INPUT::KEYS::PRIOR)
    return cursor > page ? cursor - page : 0;
  if (event.action == INPUT::KEYS::NEXT)
    return cursor + page < rows ? cursor + page : rows - 1;
  return cursor;
}

static auto steer(
  GUI::Handle document, Whole index, const INPUT::KEYS::Event &event) -> Flag {
  auto &entry = STATE::documents[document];
  const auto &node = entry.nodes[index];
  const auto rows = node.properties.rows;
  if (rows == 0) return false;
  if (event.action == INPUT::KEYS::ENTER) {
    GUI::record(entry, index, GUI::Event::ACTIVATED);
    entry.pointer.activations.push_back(node.id);
    return true;
  }
  if (!::steers(event.action)) return false;
  const auto cursor = node.properties.cursor;
  const auto moved = ::landed(event, cursor, rows, GUI::fits(document, index));
  if (moved == cursor) return true;
  GUI::set(document, node.id.c_str(), GUI::Cursor{moved});
  GUI::record(entry, index, GUI::Event::SELECTED);
  entry.pointer.selections.push_back(node.id);
  return true;
}

auto GUI::key(Handle document, const INPUT::KEYS::Event &event) -> Status {
  if (!live(document)) return 1;
  auto &entry = STATE::documents[document];
  const auto cell = entry.editing;
  if (cell != PARENTLESS && entry.nodes[cell].live) {
    const Flag took =
      entry.seated ? type(document, cell, event) : nudge(document, cell, event);
    if (took) return 0;
  }
  if (roam(document, event)) return 0;
  const auto index = ::steered(entry);
  if (index != PARENTLESS && ::steer(document, index, event)) return 0;
  record(entry, event);
  return 0;
}

void GUI::navigate() {
  if (STATE::focused == NONE) return;
  if (!live(STATE::focused)) {
    STATE::focused = NONE;
    INPUT::TEXT::focus(false);
    return;
  }
  if (!INPUT::TEXT::wanted()) {
    STATE::focused = NONE;
    return;
  }
  for (const auto &event : INPUT::TEXT::drain()) key(STATE::focused, event);
}

constexpr Float DOUBLE = 0.400f;

auto GUI::doubles(GUI::Pointer &aim, Whole index, Whole row) -> Flag {
  const Float now = CLOCK::GET::elapsed();
  const Float since = now - aim.stamp;
  const Flag pair = aim.armed == index && aim.struck == row && since >= 0.0f &&
                    since <= ::DOUBLE;
  aim.armed = index;
  aim.struck = row;
  aim.stamp = now;
  return pair;
}

static void taken(GUI::Document &entry, Whole index, Flag doubled) {
  const auto &list = entry.nodes[index];
  if (!doubled && !list.properties.quick) {
    GUI::record(entry, index, GUI::Event::SELECTED);
    entry.pointer.selections.push_back(list.id);
    return;
  }
  entry.pointer.armed = GUI::PARENTLESS;
  GUI::record(entry, index, GUI::Event::ACTIVATED);
  entry.pointer.activations.push_back(list.id);
}

void GUI::select(Handle document, Document &entry, Whole hit) {
  constexpr Whole BASE = 10;
  const auto &node = entry.nodes[hit];
  if (node.row != ::ITEM || node.parent == PARENTLESS) return;
  const auto index = node.parent;
  const auto &list = entry.nodes[index];
  if (list.row != ::LIST) return;
  const auto dot = node.id.rfind('.');
  if (dot == String::npos) return;
  const auto at =
    static_cast<Whole>(std::strtoul(node.id.c_str() + dot + 1, nullptr, BASE));
  const auto row = list.properties.first + at;
  if (row >= list.properties.rows) return;
  const Flag doubled = doubles(entry.pointer, index, row);
  if (row == list.properties.cursor) return ::taken(entry, index, doubled);
  const auto quick = list.properties.quick;
  GUI::set(document, list.id.c_str(), GUI::Cursor{row});
  record(entry, index, Event::SELECTED);
  entry.pointer.selections.push_back(list.id);
  if (!quick) return;
  record(entry, index, Event::ACTIVATED);
  entry.pointer.activations.push_back(list.id);
}

constexpr Float NOTCH = 3.0f;

auto GUI::spin(Handle document, Float wheel) -> Flag {
  if (wheel == 0.0f) return false;
  auto &entry = STATE::documents[document];
  const auto hovered = entry.pointer.hovered;
  if (hovered == PARENTLESS || !entry.nodes[hovered].live) return false;
  const auto &node = entry.nodes[hovered];
  if (node.row != ::ITEM || node.parent == PARENTLESS) return false;
  const auto &list = entry.nodes[node.parent];
  if (list.row != ::LIST) return false;
  const auto moved = static_cast<Float>(list.properties.first) - wheel * NOTCH;
  const auto first = moved <= 0.0f ? 0 : static_cast<Whole>(moved);
  GUI::set(document, list.id.c_str(), GUI::Scroll{first});
  return true;
}
