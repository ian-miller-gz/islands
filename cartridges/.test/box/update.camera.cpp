// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"
#include <island/graphics/camera.hpp>
#include <island/graphics/math.hpp>

constexpr Float FOV = 1.0f;
constexpr Float FRONT = 0.1f;
constexpr Float BACK = 10.0f;
constexpr Float YAW = 0.6f;
constexpr Float PITCH = 0.45f;
constexpr Float DISTANCE = -3.0f;

static void model(Float matrix[16], Float side) {
  GFX::MATH::reset(matrix);
  GFX::MATH::yaw(matrix, YAW);
  GFX::MATH::pitch(matrix, PITCH);
  GFX::MATH::translate(matrix, side, 0.0f, DISTANCE);
}

auto BOX::GET::camera(Float aspect, Float side) -> GFX::SHADERS::Camera {
  const auto projection = GFX::CAMERA::perspective(FOV, aspect, FRONT, BACK);
  Float turned[16];
  model(turned, side);
  GFX::SHADERS::Camera block;
  GFX::MATH::multiply(turned, projection.matrix, block.matrix);
  return block;
}

auto BOX::GET::rotation() -> GFX::SHADERS::Camera {
  GFX::SHADERS::Camera block;
  model(block.matrix, 0.0f);
  return block;
}
