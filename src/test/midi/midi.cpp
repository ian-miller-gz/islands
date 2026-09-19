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
#include <iostream>
#include "midi.internal.hpp"

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto deaf() -> Flag {
  if (MIDI::GET::ports()) return false;
  Vector<MIDI::Message> none;
  return MIDI::read(none) == 0 && MIDI::GET::ports() == 0;
}

int main() {
  Status status = check("deaf", deaf());
  status |= check("plain", plain());
  status |= check("running", running());
  status |= check("lone", lone());
  status |= check("interrupted", interrupted());
  status |= check("skipped", skipped());
  status |= check("split", split());
  status |= check("heard", heard());
  status |= check("spoken", spoken());
  return status;
}
