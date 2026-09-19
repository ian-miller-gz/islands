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
#include <test/consoles/consoles.internal.hpp>

static void echo(CONSOLES::Console &console, const String &line) {
  CONSOLES::append(console, "guest:" + line);
}

Flag guests() {
  const auto seeded = CONSOLES::GET::consoles().size();
  CONSOLES::create().shell = &echo;
  CONSOLES::create().shell = &echo;
  auto &consoles = CONSOLES::GET::consoles();
  if (consoles.size() != seeded + 2) return false;
  type(consoles[seeded], "hello");
  if (!found(consoles[seeded], "guest:hello")) return false;
  if (found(consoles[seeded + 1], "guest:hello")) return false;
  CONSOLES::remove(consoles[seeded]);
  if (consoles.size() != seeded + 1 || !consoles[seeded].guest) return false;
  CONSOLES::remove();
  if (consoles.size() != seeded) return false;
  for (const auto &console : consoles)
    if (console.guest) return false;
  return true;
}
