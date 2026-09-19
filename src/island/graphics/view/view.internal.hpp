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
#include <island/graphics/view/view.hpp>

namespace GFX::VIEW {
inline const String SHADERS = COMMON::ASSETS::SHADERS + "view";
constexpr Whole POINTS = 3;
constexpr Float NEAR = 0.0f;
constexpr Float FAR = -1.0f;

auto place() -> GFX::Viewport;
}  // namespace GFX::VIEW

namespace GFX::VIEW::STATE {
extern Extent extent;
extern Mode mode;
extern Fit fit;
extern GFX::Handle target;
extern GFX::Handle sampler;
extern GFX::Handle pipeline;
}  // namespace GFX::VIEW::STATE
