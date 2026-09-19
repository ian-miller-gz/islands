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

#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/color.hpp>
#include <island/graphics/viewports.hpp>

namespace GFX {

struct Sampling {
  GFX::Handle texture = 0;
  GFX::Handle sampler = 0;
  Flag operator==(const Sampling &) const = default;
};

struct Bindings {
  Vector<Sampling> samplers;
  Vector<GFX::Handle> buffers;
  Vector<GFX::Handle> images;
  Flag operator==(const Bindings &) const = default;
};

struct Draw {
  GFX::Handle pipeline = 0;
  Vector<Vector<Byte>> uniforms;
  Bindings vertex;
  Bindings fragment;
  GFX::Handle vertices = GFX::NONE;
  GFX::Handle indices = GFX::NONE;
  Whole first = 0;
  Whole count = 0;
  GFX::Viewport scissor = GFX::UNCLIPPED;
};

template <typename Block>
auto pack(const Block &block) -> Vector<Byte> {
  const auto *data = reinterpret_cast<const Byte *>(&block);
  return {data, data + sizeof(Block)};
}

struct Pass {
  GFX::Color clear = GFX::COLORS::BLACK;
  GFX::Viewport viewport = {};
  Vector<Draw> draws;
  GFX::Handle target = GFX::NONE;
};

}  // namespace GFX

namespace GFX::PASSES {
constexpr Float FAR = 1.0f;

void submit(Vector<GFX::Pass> passes);

void submit(GFX::Pass &&pass);

auto list() -> const Vector<GFX::Pass> &;

}  // namespace GFX::PASSES

namespace GFX {

struct Dispatch {
  GFX::Handle pipeline = 0;
  Vector<Vector<Byte>> uniforms;
  Bindings bindings;
  GFX::Group groups = {1, 1, 1};
};

}  // namespace GFX

namespace GFX::DISPATCHES {
void submit(Vector<GFX::Dispatch> dispatches);
auto list() -> const Vector<GFX::Dispatch> &;
}  // namespace GFX::DISPATCHES

namespace GFX::PASSES {

auto last() -> GFX::Pass &;
}  // namespace GFX::PASSES
