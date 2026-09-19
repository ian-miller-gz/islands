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
#include <island/input.hpp>
#include <algorithm>
#include <utility>

namespace STATE = GUI::STATE;

namespace {

auto span(const GUI::Properties &properties) -> std::pair<Whole, Whole> {
  const auto at = properties.caret;
  const auto from = properties.selection;
  return at < from ? std::pair{at, from} : std::pair{from, at};
}

void splice(
  GUI::Handle document, Whole index, const String &text, Whole first,
  Whole last) {
  auto &node = STATE::documents[document].nodes[index];
  const auto id = node.id;
  String run = node.properties.text;
  run.replace(first, last - first, text);
  const Whole landed = first + text.size();
  GUI::set(document, id.c_str(), GUI::Text{run});
  GUI::set(document, id.c_str(), GUI::Caret{landed, landed});
}

auto insert(GUI::Handle document, Whole index, Whole codepoint) -> Flag {
  constexpr Whole FIRST = 0x20, LAST = 0x7f;
  if (codepoint < FIRST || codepoint >= LAST) return false;
  const auto [first, last] =
    ::span(STATE::documents[document].nodes[index].properties);
  ::splice(
    document, index, String(1, static_cast<Char>(codepoint)), first, last);
  return true;
}

auto cut(GUI::Handle document, Whole index, Flag ahead) -> Flag {
  const auto &properties = STATE::documents[document].nodes[index].properties;
  auto [first, last] = ::span(properties);
  if (first == last) {
    if (ahead && last < properties.text.size()) last += 1;
    if (!ahead && first > 0) first -= 1;
  }
  if (first == last) return false;
  ::splice(document, index, {}, first, last);
  return true;
}

void move(GUI::Handle document, Whole index, Whole landed, Flag shift) {
  const auto &node = STATE::documents[document].nodes[index];
  const auto from = shift ? node.properties.selection : landed;
  GUI::set(document, node.id.c_str(), GUI::Caret{landed, from});
}

constexpr Whole PASTE = INPUT::BINDINGS::keys["GUI::paste"];
constexpr Whole COPY = INPUT::BINDINGS::keys["GUI::copy"];

auto chord(Whole codepoint, Whole bound) -> Flag {
  constexpr Whole SHIFT = 'a' - 'A';
  return codepoint == bound || codepoint == bound + SHIFT;
}

auto paste(GUI::Handle document, Whole index) -> Flag {
  constexpr Whole FIRST = 0x20, LAST = 0x7f;
  String kept;
  for (const Char letter : INPUT::CLIPBOARD::read()) {
    const auto byte = static_cast<unsigned char>(letter);
    if (byte >= FIRST && byte < LAST) kept += letter;
  }
  if (kept.empty()) return false;
  const auto [first, last] =
    ::span(STATE::documents[document].nodes[index].properties);
  ::splice(document, index, kept, first, last);
  return true;
}

void copy(GUI::Handle document, Whole index) {
  const auto &properties = STATE::documents[document].nodes[index].properties;
  const auto [first, last] = ::span(properties);
  if (first == last) return;
  INPUT::CLIPBOARD::write(properties.text.substr(first, last - first));
}

}  // namespace

auto GUI::ring(const Document &entry, Whole index) -> Whole {
  const auto count = entry.nodes.size();
  for (Whole step = 1; step <= count; step += 1) {
    const auto at = (index + step) % count;
    const auto &node = entry.nodes[at];
    if (!node.live || !node.properties.visible) continue;
    if (node.kind == "field" || node.kind == "dial") return at;
  }
  return index;
}

auto GUI::type(Handle document, Whole index, const INPUT::KEYS::Event &event)
  -> Flag {
  using namespace INPUT::KEYS;
  auto &entry = STATE::documents[document];
  if (
    event.action == UP || event.action == DOWN || event.action == PRIOR ||
    event.action == NEXT) {
    seat(document, PARENTLESS, false);
    return false;
  }
  const auto id = entry.nodes[index].id;
  const auto &properties = entry.nodes[index].properties;
  const auto end = properties.text.size();
  const auto at = properties.caret;
  if (event.action == ENTER) {
    record(entry, index, Event::COMMITTED);
    entry.pointer.commits.push_back(id);
    if (entry.nodes[index].kind == "dial") seat(document, index, false);
    return true;
  }
  if (event.action == TEXT && event.codepoint == TAB) {
    const auto next = ring(entry, index);
    seat(document, next, entry.nodes[next].kind == "field");
    return true;
  }
  Flag changed = false;
  if (event.action == TEXT && event.control) {
    if (::chord(event.codepoint, ::COPY))
      return (::copy(document, index), true);
    if (::chord(event.codepoint, ::PASTE))
      changed = ::paste(document, index);
    else
      return true;
  }
  if (event.action == TEXT && !event.control)
    changed = ::insert(document, index, event.codepoint);
  if (event.action == ERASE) changed = ::cut(document, index, false);
  if (event.action == DELETE) changed = ::cut(document, index, true);
  if (event.action == LEFT)
    ::move(document, index, at > 0 ? at - 1 : 0, event.shift);
  if (event.action == RIGHT)
    ::move(document, index, at < end ? at + 1 : end, event.shift);
  if (event.action == HOME) ::move(document, index, 0, event.shift);
  if (event.action == END) ::move(document, index, end, event.shift);
  if (!changed) return true;
  auto &edits = entry.pointer.edits;
  if (std::find(edits.begin(), edits.end(), id) != edits.end()) return true;
  record(entry, index, Event::EDITED);
  edits.push_back(id);
  return true;
}

auto GUI::typeable(const Node &node) -> Flag {
  return node.kind == "field" ||
         (node.kind == "dial" && !flagged(node.properties) &&
          !node.properties.graphic);
}

void GUI::seat(Handle document, Document &entry, Whole hit) {
  const auto &node = entry.nodes[hit];
  if (!typeable(node)) {
    seat(document, PARENTLESS, false);
    return;
  }
  seat(document, hit, true);
  if (node.kind == "dial") {
    const auto end = node.properties.text.size();
    GUI::set(document, node.id.c_str(), GUI::Caret{static_cast<Whole>(end), 0});
    return;
  }
  land(document, entry, hit, entry.pointer.grab.x);
}
