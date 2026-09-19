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

namespace GFX {
using Handle = Whole;
constexpr Handle NONE = static_cast<Handle>(-1);
}  // namespace GFX

namespace GFX::BUFFERS {
enum Kind { STORAGE, VERTICES, INDICES };

auto create(Whole size, Kind kind = STORAGE) -> GFX::Handle;
void destroy(GFX::Handle buffer);
void close();
}  // namespace GFX::BUFFERS

namespace GFX::BUFFERS::GET {
auto data(GFX::Handle buffer) -> void *;

template <typename Type>
auto data(GFX::Handle buffer) -> Type * {
  return static_cast<Type *>(data(buffer));
}
}  // namespace GFX::BUFFERS::GET

namespace GFX::TEXTURES {
enum Format { RGBA8, RGBA16F, RG32F };
constexpr Whole STRIDE = 4;

constexpr auto depth(Format format) -> Whole {
  return format == RGBA8 ? STRIDE : 2 * STRIDE;
}

auto create(Whole width, Whole height, Format format = RGBA8) -> GFX::Handle;
void write(GFX::Handle texture, const void *pixels);

void write(
  GFX::Handle texture, const void *pixels, Whole x, Whole y, Whole width,
  Whole height);
void destroy(GFX::Handle texture);
void close();
}  // namespace GFX::TEXTURES

namespace GFX::TARGETS {
auto create(Whole width, Whole height) -> GFX::Handle;
void destroy(GFX::Handle target);
void close();
}  // namespace GFX::TARGETS

namespace GFX::FIELDS {
auto create(Whole width, Whole height) -> GFX::Handle;
void destroy(GFX::Handle field);
void close();
}  // namespace GFX::FIELDS

namespace GFX::SAMPLERS {
enum Kind { CLAMP, REPEAT, LINEAR };

auto create(Kind kind = CLAMP) -> GFX::Handle;
void destroy(GFX::Handle sampler);
void close();
}  // namespace GFX::SAMPLERS
