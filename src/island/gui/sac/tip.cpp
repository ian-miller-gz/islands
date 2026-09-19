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
#include <island/gui/tip.internal.hpp>
#include <island/gui/events.internal.hpp>

namespace {
Map<GUI::Handle, GUI::TIP::Rest> rests;
}  // namespace

auto GUI::TIP::rested(Handle document) -> Rest & {
  Rest &entry = ::rests[document];
  const Whole current = GUI::GET::generation(document);
  if (entry.generation != current) {
    entry = {};
    entry.generation = current;
  }
  return entry;
}

void GUI::TIP::note(Handle document, STRING::Hot id, const String &line) {
  if (!live(document)) return;
  auto &notes = rested(document).notes;
  if (line.empty()) {
    notes.erase(id);
    return;
  }
  notes[id] = line;
}

auto GUI::TIP::GET::note(Handle document, STRING::Hot id) -> String {
  if (!live(document)) return {};
  const auto &notes = rested(document).notes;
  const auto found = notes.find(id);
  return found == notes.end() ? String() : found->second;
}

auto GUI::TIP::GET::notes(Handle document, const String &prefix)
  -> Vector<String> {
  Vector<String> named;
  if (!live(document)) return named;
  for (const auto &entry : rested(document).notes)
    if (entry.first.starts_with(prefix)) named.push_back(entry.first);
  return named;
}
