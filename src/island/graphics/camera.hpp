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
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/shaders/blocks.hpp>
#include <cmath>
#include <initializer_list>

namespace GFX::CAMERA {

constexpr Float NEAR = 0.0f;
constexpr Float FAR = -1.0f;

inline auto orthographic(
  Float left, Float right, Float bottom, Float top, Float near,
  Float far) -> SHADERS::Camera {
  return {{
    2.0f / (right - left),
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    2.0f / (top - bottom),
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f / (near - far),
    0.0f,
    (left + right) / (left - right),
    (top + bottom) / (bottom - top),
    near / (near - far),
    1.0f,
  }};
}

inline auto orthographic(Float left, Float right, Float bottom, Float top)
  -> SHADERS::Camera {
  return orthographic(left, right, bottom, top, NEAR, FAR);
}

inline auto perspective(Float fov, Float aspect, Float near, Float far)
  -> SHADERS::Camera {
  const Float focus = 1.0f / std::tan(fov / 2.0f);
  return {{
    focus / aspect,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    focus,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    far / (near - far),
    -1.0f,
    0.0f,
    0.0f,
    (near * far) / (near - far),
    0.0f,
  }};
}

inline void write(
  std::initializer_list<GFX::Handle> pipelines, Whole slot,
  const SHADERS::Camera &block) {
  for (const auto pipeline : pipelines)
    GFX::PIPELINES::write(pipeline, slot, &block);
}

}  // namespace GFX::CAMERA
