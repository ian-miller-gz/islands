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
#include <island/midi/backend/ports.hpp>
#include <common.hpp>

auto MIDI::BACKEND::listening() -> Whole { return 0; }

auto MIDI::BACKEND::ports() -> Whole { return 0; }

auto MIDI::BACKEND::take(Whole, Byte *, Whole) -> Whole { return 0; }

void MIDI::BACKEND::close() {}
auto MIDI::BACKEND::devices() -> Vector<Device> { return {}; }
auto MIDI::BACKEND::outlet(const String &, Whole &) -> Flag { return false; }
auto MIDI::BACKEND::put(Whole, const Byte *, Whole) -> Whole { return 0; }
void MIDI::BACKEND::shut(Whole) {}
void MIDI::BACKEND::shut() {}
void MIDI::BACKEND::rehome(const String &) {}
