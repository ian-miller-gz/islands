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

#include <island/graphics/graphics.hpp>
#include <island/graphics/backend/close.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/sprites/sprites.hpp>
#include <island/graphics/text/text.hpp>
#include <island/graphics/view/view.hpp>
#include <common.hpp>
#define LOGGER_CATEGORY

namespace GFX {
inline void close() {
  TEXT::close();
  VIEW::reset();
  SPRITES::reset();
  GFX::BACKEND::close();
}

inline void reset() {
  GFX::PASSES::submit(Vector<GFX::Pass>{});
  TEXT::close();
  VIEW::reset();
  SPRITES::reset();
  GFX::BACKEND::reset();
}
}  // namespace GFX

#undef LOGGER_CATEGORY