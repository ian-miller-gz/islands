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

namespace STATE = GUI::STATE;

namespace {

constexpr Float RATE = 0.02f;

constexpr Float NUDGES = 100.0f;

auto step(const GUI::Properties &properties) -> Float {
  if (properties.most <= properties.least) return 1.0f;
  const Float span = properties.most - properties.least;
  if (properties.steps == 0) return span / NUDGES;
  return span / static_cast<Float>(properties.steps);
}

}  // namespace

void GUI::turn(Handle document, Document &entry, Whole index, Float value) {
  const auto id = entry.nodes[index].id;
  const Float before = entry.nodes[index].properties.value;
  GUI::set(document, id.c_str(), GUI::Value{value});
  if (entry.nodes[index].properties.value == before) return;
  auto &turns = entry.pointer.turns;
  if (std::find(turns.begin(), turns.end(), id) != turns.end()) return;
  record(entry, index, Event::DIALLED);
  turns.push_back(id);
}

void GUI::drag(Handle document, Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  const auto index = pointer.pressed;
  if (index == PARENTLESS || !entry.nodes[index].live) return;
  if (entry.nodes[index].kind != "dial") return;
  if (entry.nodes[index].properties.seated) return;
  const auto &properties = entry.nodes[index].properties;
  const Float pull = x - pointer.grab.x;
  const Flag slider = properties.graphic && !flagged(properties) &&
                      properties.most > properties.least;
  if (!slider && !pointer.dragged && std::abs(pull) < GUI::THRESHOLD) return;
  if (!pointer.dragged) pointer.armed = PARENTLESS;
  pointer.dragged = true;
  if (properties.most <= properties.least) {
    turn(document, entry, index, properties.value + pull * RATE);
    return;
  }
  const auto corner = BACKEND::origin(document, index);
  const auto size = BACKEND::measured(document, index);
  if (size.w <= 0.0f) return;
  const Float across = (x - corner.x) / size.w;
  turn(
    document, entry, index,
    properties.least + across * (properties.most - properties.least));
}

auto GUI::flagged(const Properties &properties) -> Flag {
  return properties.steps == 1 && properties.most > properties.least;
}

auto GUI::flip(Handle document, Document &entry, Whole hit) -> Flag {
  if (entry.nodes[hit].kind != "dial") return false;
  const auto &properties = entry.nodes[hit].properties;
  if (!flagged(properties)) return false;
  const Float middle = (properties.least + properties.most) / 2.0f;
  const Float thrown =
    properties.value > middle ? properties.least : properties.most;
  turn(document, entry, hit, thrown);
  return true;
}

void GUI::restore(Handle document, Document &entry, Whole hit) {
  if (entry.nodes[hit].kind != "dial") return;
  if (flagged(entry.nodes[hit].properties)) return;
  if (entry.nodes[hit].properties.graphic) return;
  if (!doubles(entry.pointer, hit, 0)) return;
  entry.pointer.armed = PARENTLESS;
  turn(document, entry, hit, entry.nodes[hit].properties.resting);
}

auto GUI::nudge(Handle document, Whole index, const INPUT::KEYS::Event &event)
  -> Flag {
  using namespace INPUT::KEYS;
  auto &entry = STATE::documents[document];
  if (
    event.action == UP || event.action == DOWN || event.action == PRIOR ||
    event.action == NEXT) {
    seat(document, PARENTLESS, false);
    return false;
  }
  if (event.action == ENTER) {
    if (flip(document, entry, index)) return true;
    if (!typeable(entry.nodes[index])) return true;
    seat(document, index, true);
    const auto end = entry.nodes[index].properties.text.size();
    GUI::set(
      document, entry.nodes[index].id.c_str(),
      GUI::Caret{static_cast<Whole>(end), 0});
    return true;
  }
  if (event.action == TEXT && event.codepoint == TAB) {
    const auto next = ring(entry, index);
    seat(document, next, entry.nodes[next].kind == "field");
    return true;
  }
  const auto &properties = entry.nodes[index].properties;
  const Flag bound = properties.most > properties.least;
  if (event.action == LEFT)
    turn(document, entry, index, properties.value - ::step(properties));
  if (event.action == RIGHT)
    turn(document, entry, index, properties.value + ::step(properties));
  if (event.action == HOME && bound)
    turn(document, entry, index, properties.least);
  if (event.action == END && bound)
    turn(document, entry, index, properties.most);
  return true;
}
