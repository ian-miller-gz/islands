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

#include <island/graphics/backend/passes.hpp>
#include <island/graphics/shaders/blocks.hpp>

namespace GFX::VIEW {

enum Mode { COMPOSED, DIRECT };

enum Fit { INTEGER, LETTERBOX };

struct Extent {
  Whole width = 0;
  Whole height = 0;
};

void create(Whole width, Whole height, Mode mode = COMPOSED, Fit fit = INTEGER);

void open(GFX::Pass &pass);

void close(Vector<GFX::Pass> &passes);

void reset();

}  // namespace GFX::VIEW

namespace GFX::VIEW::GET {
auto extent() -> Extent;
auto scale() -> Float;
auto viewport() -> GFX::Viewport;
auto camera() -> GFX::SHADERS::Camera;
}  // namespace GFX::VIEW::GET
