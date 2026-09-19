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
#include <island/gui/sac.hpp>
#include <island/gui/stroke.internal.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
Map<GUI::Handle, GUI::SAC::STROKE::Drawing> standing;
Flag told = false;
}  // namespace

auto GUI::SAC::STROKE::drawings() -> Map<Handle, Drawing> & {
  return ::standing;
}

auto GUI::SAC::STROKE::drawing(Handle document) -> Drawing & {
  Drawing &entry = ::standing[document];
  const Whole current = GUI::GET::generation(document);
  if (entry.generation != current) {
    entry = {};
    entry.generation = current;
  }
  return entry;
}

auto GUI::SAC::STROKE::refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::STROKE::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "stroke")
      .error(
        "The stroke is not designated: add `stroke: true` under the "
        "manifest's `gui:` block (or adopt it through GUI::SAC::adopt).");
  ::told = true;
  return true;
}

auto GUI::SAC::STROKE::draw(Handle document, const Mode &mode) -> Status {
  if (!live(document)) return 1;
  if (refused()) return 1;
  if (mode.cell.w <= 0.0f || mode.cell.h <= 0.0f) return 1;
  Drawing &entry = drawing(document);
  if (entry.mode.board != mode.board) {
    entry.gesture = {};
    entry.ledger.clear();
  }
  entry.mode = mode;
  return 0;
}

auto GUI::SAC::STROKE::GET::mode(Handle document) -> STRING::Cold {
  if (!live(document)) return {};
  const Drawing &entry = drawing(document);
  return entry.mode.board.empty() ? STRING::Cold() : entry.mode.name;
}

auto GUI::SAC::STROKE::GET::gesture(Handle document) -> Gesture {
  if (!live(document)) return {};
  return drawing(document).gesture;
}

auto GUI::SAC::STROKE::GET::laid(Handle document) -> Vector<Cell> {
  if (!live(document)) return {};
  return drawing(document).laid;
}
