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
#include <island/gui/sac.hpp>
#include <island/gui/rows.hpp>
#include <island/gui/carry.hpp>
#include <island/gui/dialog.hpp>
#include <island/gui/ladder.hpp>
#include <island/gui/playhead.hpp>
#include <island/gui/seat.hpp>
#include <island/gui/window.hpp>
#include <island/gui/stroke.hpp>
#include <island/gui/tip.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
constexpr STRING::Hot NAMES[] = {
  GUI::DIALOG::NAME,      GUI::TIP::NAME,           GUI::SAC::LADDER::NAME,
  GUI::SAC::WINDOW::NAME, GUI::SAC::CARRY::NAME,    GUI::SAC::STROKE::NAME,
  GUI::SAC::ROWS::NAME,   GUI::SAC::PLAYHEAD::NAME, GUI::SAC::SEAT::NAME};

Vector<String> designations;
}  // namespace

auto GUI::SAC::adopt(const Vector<String> &names) -> Status {
  auto &logger = LOGGER::get(category + "sac");
  designations.clear();
  Status refused = 0;
  for (const auto &name : names) {
    Flag known = false;
    for (const auto &carried : NAMES)
      if (name == carried) known = true;
    if (!known) {
      logger.error(
        "`gui:` designates \"" + name +
        "\", which names no component this engine carries.");
      refused += 1;
      continue;
    }
    designations.push_back(name);
  }
  return refused;
}

auto GUI::SAC::GET::adopted(const String &name) -> Flag {
  for (const auto &designation : designations)
    if (designation == name) return true;
  return false;
}
