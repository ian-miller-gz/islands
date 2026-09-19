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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

template <typename Type>
using Vector = std::vector<Type>;
template <typename Key, typename Value>
using Map = std::unordered_map<Key, Value>;

using Char = char;
using Byte = char;
using Bool = bool;
using Flag = bool;
using Whole = std::size_t;
using Integer = int_fast32_t;
using Word = uint32_t;
using Float = float;
using String = std::string;
using Status = int;

namespace STRING {
using Hot = const char *;
using Cold = String;
template <unsigned int size>
using Buffer = const char[size];
}  // namespace STRING

// End of types.hpp