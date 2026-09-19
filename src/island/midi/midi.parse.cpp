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
#include <common.hpp>

namespace {
constexpr Whole DATA = 0x80;
constexpr Whole SYSTEM = 0xF0;
constexpr Whole REALTIME = 0xF8;
constexpr Whole KIND = 0xF0;
constexpr Whole CHANNEL = 0x0F;

auto lone(Whole status) -> Flag {
  const Whole kind = status & KIND;
  return kind == MIDI::Message::PROGRAM || kind == MIDI::Message::PRESSURE;
}

auto step(MIDI::Parser &parser, Whole byte, MIDI::Message *into) -> Whole {
  if (byte >= REALTIME) return 0;
  if (byte >= DATA) {
    parser.status = byte < SYSTEM ? byte : 0;
    parser.gathering = false;
    return 0;
  }
  if (!parser.status) return 0;
  if (!lone(parser.status) && !parser.gathering) {
    parser.first = byte;
    parser.gathering = true;
    return 0;
  }
  *into = {
    .kind = parser.status & KIND,
    .channel = parser.status & CHANNEL,
    .first = parser.gathering ? parser.first : byte,
    .second = parser.gathering ? byte : 0};
  parser.gathering = false;
  return 1;
}
}  // namespace

auto MIDI::parse(Parser &parser, const Byte *from, Whole count, Message *into)
  -> Whole {
  Whole written = 0;
  for (Whole at = 0; at < count; at += 1)
    written +=
      step(parser, static_cast<unsigned char>(from[at]), into + written);
  return written;
}
