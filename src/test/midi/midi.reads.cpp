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
#include <island/midi/midi.internal.hpp>
#include "midi.internal.hpp"

namespace {
constexpr Whole SEATS = 8;

auto through(
  MIDI::Parser &parser, const Vector<Whole> &tape,
  MIDI::Message *into) -> Whole {
  Vector<Byte> raw;
  for (const Whole byte : tape) raw.push_back(static_cast<Byte>(byte));
  return MIDI::parse(parser, raw.data(), raw.size(), into);
}

auto matches(
  const MIDI::Message &message, Whole kind, Whole channel, Whole first,
  Whole second) -> Flag {
  return message.kind == kind && message.channel == channel &&
         message.first == first && message.second == second;
}
}  // namespace

auto plain() -> Flag {
  MIDI::Parser parser;
  MIDI::Message out[SEATS];
  return through(parser, {0x92, 0x3C, 0x40}, out) == 1 &&
         matches(out[0], MIDI::Message::NOTE_ON, 2, 0x3C, 0x40);
}

auto running() -> Flag {
  MIDI::Parser parser;
  MIDI::Message out[SEATS];
  return through(parser, {0x90, 0x3C, 0x40, 0x3E, 0x50}, out) == 2 &&
         matches(out[0], MIDI::Message::NOTE_ON, 0, 0x3C, 0x40) &&
         matches(out[1], MIDI::Message::NOTE_ON, 0, 0x3E, 0x50);
}

auto lone() -> Flag {
  MIDI::Parser parser;
  MIDI::Message out[SEATS];
  return through(parser, {0xC3, 0x05, 0xD1, 0x40}, out) == 2 &&
         matches(out[0], MIDI::Message::PROGRAM, 3, 0x05, 0) &&
         matches(out[1], MIDI::Message::PRESSURE, 1, 0x40, 0);
}

auto interrupted() -> Flag {
  MIDI::Parser parser;
  MIDI::Message out[SEATS];
  return through(parser, {0x90, 0x3C, 0xF8, 0x40}, out) == 1 &&
         matches(out[0], MIDI::Message::NOTE_ON, 0, 0x3C, 0x40);
}

auto skipped() -> Flag {
  MIDI::Parser parser;
  MIDI::Message out[SEATS];
  const Whole count = through(
    parser,
    {0x90, 0x3C, 0x40, 0xF0, 0x7E, 0x01, 0xF7, 0x3E, 0x50, 0x90, 0x40, 0x60},
    out);
  return count == 2 && matches(out[0], MIDI::Message::NOTE_ON, 0, 0x3C, 0x40) &&
         matches(out[1], MIDI::Message::NOTE_ON, 0, 0x40, 0x60);
}

auto split() -> Flag {
  MIDI::Parser parser;
  MIDI::Message out[SEATS];
  return through(parser, {0x90, 0x3C}, out) == 0 &&
         through(parser, {0x40}, out) == 1 &&
         matches(out[0], MIDI::Message::NOTE_ON, 0, 0x3C, 0x40);
}
