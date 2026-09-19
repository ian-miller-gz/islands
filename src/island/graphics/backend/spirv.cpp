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
#include <island.hpp>
#include <island/graphics/backend/spirv.hpp>
#include <cstring>

static constexpr Whole WORD = 4;
static constexpr Whole HEADER = 5 * WORD;
static constexpr uint32_t MAGIC = 0x07230203;

auto GFX::BACKEND::SPIRV::vet(const Vector<Byte> &bytes) -> Flag {
  if (bytes.size() < HEADER || bytes.size() % WORD != 0) return false;
  uint32_t magic = 0;
  std::memcpy(&magic, bytes.data(), sizeof(magic));
  return magic == MAGIC;
}
