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
#include <island/gui/nga.internal.hpp>

namespace {

struct Step {
  INPUT::KEYS::Action action;
  Float across;
  Float down;
};
constexpr Step STEPS[] = {
  {INPUT::KEYS::LEFT, -1.0f, 0.0f}, {INPUT::KEYS::RIGHT, 1.0f, 0.0f},
  {INPUT::KEYS::UP, 0.0f, -1.0f},   {INPUT::KEYS::DOWN, 0.0f, 1.0f},
  {INPUT::KEYS::HOME, 0.0f, 0.0f},  {INPUT::KEYS::END, 0.0f, 0.0f}};

auto standing(const GUI::Document &entry) -> Whole {
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    if (entry.nodes[index].live && entry.nodes[index].properties.selected)
      return index;
  return GUI::PARENTLESS;
}

auto edging(const GUI::Document &entry, Whole deck, Flag last) -> Whole {
  Whole found = GUI::PARENTLESS;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (!GUI::NGA::selectable(entry, index) || GUI::board(entry, index) != deck)
      continue;
    if (found == GUI::PARENTLESS || last) found = index;
  }
  return found;
}

auto nearest(
  GUI::Handle document, const GUI::Document &entry, Whole from,
  const Step &step) -> Whole {
  const auto deck = GUI::board(entry, from);
  const auto here = GUI::NGA::centre(document, from);
  Whole found = GUI::PARENTLESS;
  Float best = 0.0f;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (index == from || !GUI::NGA::selectable(entry, index)) continue;
    if (GUI::board(entry, index) != deck) continue;
    const auto there = GUI::NGA::centre(document, index);
    const Float x = there.x - here.x;
    const Float y = there.y - here.y;
    const Float along = x * step.across + y * step.down;
    const Float aside = x * step.down - y * step.across;
    if (along <= 0.0f) continue;
    const Float score = along + (aside < 0.0f ? -aside : aside);
    if (found != GUI::PARENTLESS && score >= best) continue;
    found = index;
    best = score;
  }
  return found;
}

}  // namespace

auto GUI::roam(Handle document, const INPUT::KEYS::Event &event) -> Flag {
  auto &entry = STATE::documents[document];
  const auto from = ::standing(entry);
  if (from == PARENTLESS) return false;
  const auto deck = board(entry, from);
  if (deck == PARENTLESS) return false;
  if (event.action == INPUT::KEYS::ENTER) {
    record(entry, deck, Event::ACTIVATED);
    entry.pointer.activations.push_back(entry.nodes[deck].id);
    return true;
  }
  for (const auto &step : ::STEPS) {
    if (step.action != event.action) continue;
    const Flag jump = step.across == 0.0f && step.down == 0.0f;
    const auto landed =
      jump ? ::edging(entry, deck, event.action == INPUT::KEYS::END)
           : ::nearest(document, entry, from, step);
    if (landed != PARENTLESS) NGA::select(document, entry, deck, {landed});
    return true;
  }
  return false;
}
