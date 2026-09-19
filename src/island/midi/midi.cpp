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
#include <island/midi/midi.hpp>
#include <island/midi/midi.internal.hpp>
#include <island/midi/backend/ports.hpp>
#include <common.hpp>

namespace {
constexpr Whole BITE = 128;
Vector<MIDI::Parser> parsers;
}  // namespace

auto MIDI::read(Vector<Message> &into) -> Whole {
  if (into.empty()) return 0;
  const Whole ports = BACKEND::listening();
  if (parsers.size() != ports) parsers.assign(ports, Parser{});
  Whole count = 0;
  Byte raw[BITE];
  for (Whole port = 0; port < ports; port += 1)
    while (count < into.size()) {
      const Whole room = into.size() - count;
      const Whole ask = room < BITE ? room : BITE;
      const Whole took = BACKEND::take(port, raw, ask);
      count += parse(parsers[port], raw, took, into.data() + count);
      if (took < ask) break;
    }
  return count;
}

void MIDI::close() {
  BACKEND::close();
  parsers.clear();
  OUTPUT::remove();
}

auto MIDI::GET::ports() -> Whole { return BACKEND::ports(); }

void MIDI::home(const String &path) { BACKEND::rehome(path); }
