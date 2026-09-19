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
#include <island/graphics/camera.hpp>
#include <island/graphics/math.hpp>
#include <algorithm>
#include <cmath>

namespace GFX::CAMERA {

struct Framing {
  Float fov = 1.0f;
  Float near = 0.01f;
  Float far = 100.0f;
  Float yaw = 0.6f;
  Float pitch = 0.4f;
  Float distance = 2.5f;
  Float speed = 0.01f;
  Float zoom = 0.9f;
  Float tilt = 1.5f;
  Float least = 0.2f;
  Float most = 50.0f;
};

struct Orbit {
  Framing framing;
  Float yaw = 0.6f;
  Float pitch = 0.4f;
  Float distance = 2.5f;
  Float radius = 1.0f;
  Float center[3] = {0.0f, 0.0f, 0.0f};

  auto camera(Float aspect) const -> SHADERS::Camera {
    const auto projection = GFX::CAMERA::perspective(
      framing.fov, aspect, radius * framing.near, radius * framing.far);
    Float turned[16];
    GFX::MATH::reset(turned);
    GFX::MATH::yaw(turned, yaw);
    GFX::MATH::pitch(turned, pitch);
    Float moved[16];
    GFX::MATH::reset(moved);
    GFX::MATH::translate(moved, -center[0], -center[1], -center[2]);
    GFX::MATH::compose(moved, turned);
    GFX::MATH::translate(moved, 0.0f, 0.0f, -distance);
    GFX::SHADERS::Camera block;
    GFX::MATH::multiply(moved, projection.matrix, block.matrix);
    return block;
  }

  void right(Float out[3]) const {
    out[0] = std::cos(yaw);
    out[1] = 0.0f;
    out[2] = std::sin(yaw);
  }

  void up(Float out[3]) const {
    out[0] = std::sin(yaw) * std::sin(pitch);
    out[1] = std::cos(pitch);
    out[2] = -std::cos(yaw) * std::sin(pitch);
  }

  void forward(Float out[3]) const {
    out[0] = std::sin(yaw) * std::cos(pitch);
    out[1] = -std::sin(pitch);
    out[2] = -std::cos(yaw) * std::cos(pitch);
  }

  void home() {
    yaw = framing.yaw;
    pitch = framing.pitch;
    distance = radius * framing.distance;
  }

  void turn(Float dx, Float dy) {
    yaw += dx * framing.speed;
    pitch = std::clamp(pitch + dy * framing.speed, -framing.tilt, framing.tilt);
  }

  void dolly(Float wheel) {
    if (wheel == 0.0f) return;
    distance = std::clamp(
      distance * std::pow(framing.zoom, wheel), radius * framing.least,
      radius * framing.most);
  }
};

}  // namespace GFX::CAMERA
