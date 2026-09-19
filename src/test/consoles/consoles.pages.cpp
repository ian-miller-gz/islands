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
#include <string>

Flag pages() {
  auto &console = CONSOLES::create();
  console.rows = 4;
  for (Whole i = 0; i < 20; i += 1)
    CONSOLES::append(console, "line " + std::to_string(i));
  if (console.scroll) return false;
  CONSOLES::page(console, 1);
  if (console.scroll != 4) return false;
  CONSOLES::append(console, "arrival");
  if (console.scroll != 5) return false;
  CONSOLES::page(console, 100);
  if (console.scroll != console.lines.size() - console.rows) return false;
  CONSOLES::page(console, -100);
  if (console.scroll) return false;
  CONSOLES::submit(console);
  CONSOLES::page(console, 2);
  CONSOLES::submit(console);
  const Flag home = console.scroll == 0;
  CONSOLES::remove(console);
  return home;
}
