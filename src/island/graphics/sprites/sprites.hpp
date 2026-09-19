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

namespace GFX::SPRITES {

using Handle = Whole;

constexpr Handle NONE = static_cast<Handle>(-1);

struct Frame {
  Float u = 0.0f;
  Float v = 0.0f;
  Float w = 0.0f;
  Float h = 0.0f;
  auto operator==(const Frame &) const -> Bool = default;
};

struct Sprite {
  Float x = 0.0f;
  Float y = 0.0f;
  Float z = 0.0f;
  Float rotation = 0.0f;
  Float w = 0.0f;
  Float h = 0.0f;
  Float padding[2] = {};
  Frame texture = {0.0f, 0.0f, 1.0f, 1.0f};
  GFX::Color color = GFX::COLORS::WHITE;
};

static_assert(
  sizeof(Sprite) == 16 * sizeof(Float),
  "the sprite element is the shader pair's 16-float ABI "
  "(assets/shaders/src/sprites.vertex.hlsl)");

auto create(Handle atlas, Whole limit, Flag snap = false) -> Handle;

void push(Handle batch, Sprite sprite);

void push(
  Handle batch, Float x, Float y, Float w, Float h, Frame frame,
  GFX::Color color = GFX::COLORS::WHITE, Flag flipX = false,
  Flag flipY = false);

void rect(Handle batch, Float x, Float y, Float w, Float h, GFX::Color color);

void cell(
  Handle batch, Float x, Float y, Float w, Float h, Handle atlas, Whole column,
  Whole row, GFX::Color color = GFX::COLORS::WHITE);

void write(Handle batch, const GFX::SHADERS::Camera &camera);

auto screen() -> GFX::SHADERS::Camera;

void flush(Handle batch, GFX::Pass &pass);

void reset();

}  // namespace GFX::SPRITES

namespace GFX::SPRITES::GET {
auto limit(Handle batch) -> Whole;
}  // namespace GFX::SPRITES::GET

namespace GFX::SPRITES::ATLASES {

struct Cell {
  Whole width = 0;
  Whole height = 0;
};

struct Point {
  Whole x = 0;
  Whole y = 0;
};

struct Placement {
  Frame texture;
  Cell extent;
  Point trim;
  Cell source;
  Point pivot;
};

struct Mask {
  Vector<Byte> coverage;
};

auto create(Whole width, Whole height, Cell cell, const Vector<Byte> &rgba)
  -> Handle;

auto create(STRING::Hot path, Cell cell) -> Handle;

auto create(Cell cell, const Vector<Mask> &cells) -> Handle;

auto create(STRING::Hot path) -> Handle;

auto white() -> Handle;

auto frame(Handle atlas, Whole column, Whole row) -> Frame;

auto frame(Handle atlas, STRING::Hot name) -> Frame;

}  // namespace GFX::SPRITES::ATLASES

namespace GFX::SPRITES::ATLASES::GET {

struct Extent {
  Whole width = 0;
  Whole height = 0;
};

auto extent(Handle atlas) -> Extent;

auto sampling(Handle atlas) -> GFX::Sampling;

auto names(Handle atlas) -> Vector<String>;

auto placement(Handle atlas, STRING::Hot name) -> Placement;

}  // namespace GFX::SPRITES::ATLASES::GET
