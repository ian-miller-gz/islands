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
#include <cartridge.hpp>
#include <common.hpp>
#include <shell/engine/engine.internal.hpp>

void SHELL::ENGINE::ACTIONS::status(Session &session) {
  session.print("Bundle: " + CARTRIDGE::GET::bundle());
  session.print("Home: " + COMMON::HOME);
  String lane;
  for (const auto &preference : CARTRIDGE::GET::preferences()) {
    if (!lane.empty()) lane += ' ';
    lane += preference.name + '=' + preference.standing;
  }
  session.print("Preferences: " + (lane.empty() ? String("(none)") : lane));
  if (host.status) host.status(session);
}
