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

namespace STRING {

auto trim(const String &text) -> String;

auto split(const String &text, Char delimiter) -> Vector<String>;

auto split(const String &text, const String &separator) -> Vector<String>;

template <typename Number>
auto number(const String &text, Number fallback = Number{}) -> Number;

auto stem(const String &path) -> String;

auto pair(const String &line, String &key, String &value) -> Flag;

}  // namespace STRING
