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
#include <charconv>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/sac.hpp>
#include <island/gui/seat.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
constexpr STRING::Hot DOT = ".";

Flag told = false;

auto refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::SEAT::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "seat")
      .error(
        "The seat is not designated: add `seat: true` under the manifest's "
        "`gui:` block (or adopt it through GUI::SAC::adopt).");
  ::told = true;
  return true;
}
}  // namespace

auto GUI::SAC::SEAT::named(STRING::Hot board, Whole row) -> String {
  return std::format("{}{}{}", board, ::DOT, row);
}

auto GUI::SAC::SEAT::row(const String &id, STRING::Hot board) -> Whole {
  const String stem = String(board) + ::DOT;
  if (!id.starts_with(stem)) return NONE;
  const String digits =
    id.substr(stem.size(), id.find(::DOT, stem.size()) - stem.size());
  const char *last = digits.data() + digits.size();
  Whole seated = 0;
  const auto read = std::from_chars(digits.data(), last, seated);
  return read.ec == std::errc{} && read.ptr == last ? seated : NONE;
}

auto GUI::SAC::SEAT::selected(Handle document, STRING::Hot board) -> Whole {
  if (::refused()) return NONE;
  const Vector<STRING::Cold> standing = NGA::GET::selections(document, board);
  return standing.empty() ? NONE : row(String(standing.front()), board);
}
