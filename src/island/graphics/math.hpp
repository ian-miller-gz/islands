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
#include <cmath>

namespace GFX::MATH {

inline void multiply(const Float a[16], const Float b[16], Float out[16]) {
  for (Whole row = 0; row < 4; row += 1)
    for (Whole column = 0; column < 4; column += 1) {
      out[row * 4 + column] = 0.0f;
      for (Whole i = 0; i < 4; i += 1)
        out[row * 4 + column] += a[row * 4 + i] * b[i * 4 + column];
    }
}

inline void reset(Float matrix[16]) {
  for (Whole row = 0; row < 4; row += 1)
    for (Whole column = 0; column < 4; column += 1)
      matrix[row * 4 + column] = row == column ? 1.0f : 0.0f;
}

inline void compose(Float matrix[16], const Float other[16]) {
  Float product[16];
  multiply(matrix, other, product);
  for (Whole i = 0; i < 16; i += 1) matrix[i] = product[i];
}

inline void translate(Float matrix[16], Float x, Float y, Float z) {
  Float mover[16];
  reset(mover);
  mover[3 * 4 + 0] = x, mover[3 * 4 + 1] = y, mover[3 * 4 + 2] = z;
  compose(matrix, mover);
}

inline void scale(Float matrix[16], Float x, Float y, Float z) {
  Float sized[16];
  reset(sized);
  sized[0 * 4 + 0] = x, sized[1 * 4 + 1] = y, sized[2 * 4 + 2] = z;
  compose(matrix, sized);
}

inline void yaw(Float matrix[16], Float angle) {
  Float swung[16];
  reset(swung);
  swung[0 * 4 + 0] = std::cos(angle), swung[0 * 4 + 2] = -std::sin(angle);
  swung[2 * 4 + 0] = std::sin(angle), swung[2 * 4 + 2] = std::cos(angle);
  compose(matrix, swung);
}

inline void pitch(Float matrix[16], Float angle) {
  Float tipped[16];
  reset(tipped);
  tipped[1 * 4 + 1] = std::cos(angle), tipped[1 * 4 + 2] = std::sin(angle);
  tipped[2 * 4 + 1] = -std::sin(angle), tipped[2 * 4 + 2] = std::cos(angle);
  compose(matrix, tipped);
}

inline void roll(Float matrix[16], Float angle) {
  Float spun[16];
  reset(spun);
  spun[0 * 4 + 0] = std::cos(angle), spun[0 * 4 + 1] = std::sin(angle);
  spun[1 * 4 + 0] = -std::sin(angle), spun[1 * 4 + 1] = std::cos(angle);
  compose(matrix, spun);
}

inline void trs(
  const Float position[3], const Float euler[3], const Float extent[3],
  Float out[16]) {
  reset(out);
  scale(out, extent[0], extent[1], extent[2]);
  pitch(out, euler[0]);
  yaw(out, euler[1]);
  roll(out, euler[2]);
  translate(out, position[0], position[1], position[2]);
}

inline auto dot(const Float a[3], const Float b[3]) -> Float {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline void cross(const Float a[3], const Float b[3], Float out[3]) {
  out[0] = a[1] * b[2] - a[2] * b[1];
  out[1] = a[2] * b[0] - a[0] * b[2];
  out[2] = a[0] * b[1] - a[1] * b[0];
}

inline void normalize(Float vector[3]) {
  const Float length = std::sqrt(dot(vector, vector));
  if (length == 0.0f) return;
  for (Whole i = 0; i < 3; i += 1) vector[i] /= length;
}

inline void view(
  Float matrix[16], const Float eye[3], const Float target[3],
  const Float up[3]) {
  Float forward[3] = {
    target[0] - eye[0], target[1] - eye[1], target[2] - eye[2]};
  normalize(forward);
  Float side[3];
  cross(forward, up, side);
  normalize(side);
  Float above[3];
  cross(side, forward, above);
  Float looked[16];
  reset(looked);
  for (Whole row = 0; row < 3; row += 1) {
    looked[row * 4 + 0] = side[row];
    looked[row * 4 + 1] = above[row];
    looked[row * 4 + 2] = -forward[row];
  }
  looked[3 * 4 + 0] = -dot(side, eye);
  looked[3 * 4 + 1] = -dot(above, eye);
  looked[3 * 4 + 2] = dot(forward, eye);
  compose(matrix, looked);
}

}  // namespace GFX::MATH
