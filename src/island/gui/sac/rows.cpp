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
#include <island/gui/rows.hpp>
#include <island/gui/sac.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
Flag told = false;

auto refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::ROWS::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "rows")
      .error(
        "The rows are not designated: add `rows: true` under the "
        "manifest's `gui:` block (or adopt them through GUI::SAC::adopt).");
  ::told = true;
  return true;
}
}  // namespace

auto GUI::SAC::ROWS::stack(const Vector<Row> &rows, Float margin, Float gap)
  -> Vector<Rail> {
  Vector<Rail> rails;
  if (::refused()) return rails;
  Float top = margin;
  for (const Row &row : rows) {
    if (row.parted && !rails.empty()) top += gap;
    rails.push_back({top, row.depth});
    top += row.depth;
  }
  return rails;
}

auto GUI::SAC::ROWS::deep(const Vector<Rail> &rails, Float margin) -> Float {
  if (rails.empty()) return margin;
  return rails.back().top + rails.back().depth;
}

auto GUI::SAC::ROWS::at(const Vector<Rail> &rails, Float down) -> Whole {
  for (Whole row = 0; row < rails.size(); ++row)
    if (down >= rails[row].top && down < rails[row].top + rails[row].depth)
      return row;
  return NONE;
}

auto GUI::SAC::ROWS::reach(Float pan, Float seen) -> Float {
  return pan + seen;
}

auto GUI::SAC::ROWS::reach(Float reached, Float pan, Float seen, Float spare)
  -> Float {
  const Float sailed = pan + seen;
  return sailed > reached ? sailed + seen * spare : reached;
}
