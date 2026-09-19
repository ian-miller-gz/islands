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
#include <island/gui/backend.hpp>
#include <cmath>

namespace NGA = GUI::NGA;

namespace {

auto around(GUI::Handle document, const GUI::Document &entry, Whole index)
  -> NGA::Span {
  const auto home = GUI::box(entry, index);
  const auto corner = GUI::BACKEND::origin(document, home);
  const auto size = GUI::BACKEND::measured(document, home);
  return {corner.x, corner.y, size.w, size.h};
}

auto touched(
  GUI::Handle document, const GUI::Document &entry, Whole deck,
  const NGA::Link &link, GUI::Position from, GUI::Position to) -> Flag {
  const auto source = entry.ids.find(link.from);
  const auto sink = entry.ids.find(link.to);
  if (source == entry.ids.end() || sink == entry.ids.end()) return false;
  if (GUI::board(entry, source->second) != deck) return false;
  NGA::Span run[NGA::SEGMENTS];
  NGA::route(
    {NGA::centre(document, source->second), NGA::rank(entry, source->second),
     ::around(document, entry, source->second)},
    {NGA::centre(document, sink->second), NGA::rank(entry, sink->second),
     ::around(document, entry, sink->second)},
    NGA::THICKNESS * NGA::scaled(document, deck).w, run);
  for (const auto &span : run)
    if (NGA::crosses(span, from, to)) return true;
  return false;
}

}  // namespace

auto GUI::spanned(
  Handle document, const Document &entry, Whole deck, Float x,
  Float y) -> Flag {
  const Position at = {x, y};
  for (const auto &link : entry.links)
    if (::touched(document, entry, deck, link, at, at)) return true;
  return false;
}

void GUI::sever(Handle document, Document &entry, Float x, Float y) {
  const auto deck = entry.pointer.cutting;
  if (deck == PARENTLESS) return;
  const auto from = entry.pointer.grab;
  if (std::abs(x - from.x) < THRESHOLD && std::abs(y - from.y) < THRESHOLD)
    return;
  const Position to = {x, y};
  Vector<NGA::Link> struck;
  for (const auto &link : entry.links)
    if (::touched(document, entry, deck, link, from, to))
      struck.push_back(link);
  for (const auto &link : struck) {
    if (NGA::disconnect(document, link.from.c_str(), link.to.c_str()) != 0)
      continue;
    entry.pointer.cuts.push_back(link);
    record(entry, deck, Event::CUT);
  }
}
