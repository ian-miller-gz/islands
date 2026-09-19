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
#include <island/midi/backend/ports.hpp>
#include <common.hpp>
#include <logger.hpp>

static const String category = "~/island/midi::";

namespace {

constexpr Whole LONGEST = 3;
constexpr Whole CHANNELS = 0x0F;

auto encoded(const MIDI::Message &message, Byte *into) -> Whole {
  into[0] = static_cast<Byte>(message.kind | (message.channel & CHANNELS));
  into[1] = static_cast<Byte>(message.first);
  if (
    message.kind == MIDI::Message::PROGRAM ||
    message.kind == MIDI::Message::PRESSURE)
    return 2;
  into[2] = static_cast<Byte>(message.second);
  return LONGEST;
}

}  // namespace

auto MIDI::OUTPUT::create(const String &device) -> Handle {
  for (const Device &row : BACKEND::devices()) {
    if (row.name != device) continue;
    Whole outlet = 0;
    if (!BACKEND::outlet(row.node, outlet)) return NONE;
    return outlet + 1;
  }
  LOGGER::get(category + "create")
    .warn("No MIDI device named %s", device.c_str());
  return NONE;
}

void MIDI::OUTPUT::send(Handle output, const Message &message) {
  if (output == NONE) return;
  Byte wire[LONGEST];
  const Whole count = ::encoded(message, wire);
  BACKEND::put(output - 1, wire, count);
}

void MIDI::OUTPUT::remove(Handle output) {
  if (output != NONE) BACKEND::shut(output - 1);
}

void MIDI::OUTPUT::remove() { BACKEND::shut(); }

auto MIDI::GET::devices() -> Vector<Device> { return BACKEND::devices(); }
