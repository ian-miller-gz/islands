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

#include <common.hpp>

namespace CONDUIT {
using Handle = Whole;
constexpr Handle NONE = static_cast<Handle>(-1);

auto create(const String &name, Whole records, Whole stride) -> Handle;

auto open(const String &name, Whole records, Whole stride) -> Handle;

auto write(Handle conduit, const void *record) -> Flag;

auto read(Handle conduit, void *record) -> Flag;

void destroy(Handle conduit);
}  // namespace CONDUIT

namespace CONDUIT::GET {
auto records(Handle conduit) -> Whole;
auto stride(Handle conduit) -> Whole;
}  // namespace CONDUIT::GET
