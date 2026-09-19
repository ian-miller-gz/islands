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
#pragma once

#include <common/types.hpp>

namespace MIDI {

struct Message {
  enum Kind : Whole {
    NOTE_OFF = 0x80,
    NOTE_ON = 0x90,
    TOUCH = 0xA0,
    CONTROL = 0xB0,
    PROGRAM = 0xC0,
    PRESSURE = 0xD0,
    BEND = 0xE0
  };
  Whole kind = NOTE_OFF;
  Whole channel = 0;
  Whole first = 0;
  Whole second = 0;
};

auto read(Vector<Message> &into) -> Whole;

void home(const String &path);

void close();

struct Device {
  String name;
  String node;
};

using Handle = Whole;

namespace OUTPUT {
constexpr Handle NONE = 0;
auto create(const String &device) -> Handle;
void send(Handle output, const Message &message);
void remove(Handle output);
void remove();
}  // namespace OUTPUT

namespace GET {
auto ports() -> Whole;
auto devices() -> Vector<Device>;
}  // namespace GET

}  // namespace MIDI
