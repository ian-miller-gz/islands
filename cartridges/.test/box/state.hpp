// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/interface.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/shaders/blocks.hpp>
#include <island/graphics/model/model.hpp>

namespace BOX {

extern GFX::MODEL::Scene scene;
extern GFX::Handle sampler;
extern GFX::Pass pass;

constexpr Float SPREAD = 1.1f;

void create();
void update();
}  // namespace BOX

namespace BOX::GET {
auto camera(Float aspect, Float side) -> GFX::SHADERS::Camera;
auto rotation() -> GFX::SHADERS::Camera;
}  // namespace BOX::GET

namespace BOX::PIPELINES {
extern GFX::Handle unlit;
extern GFX::Handle pbr;
}  // namespace BOX::PIPELINES

namespace BOX::PBR {
constexpr Whole CAMERA = 0;
constexpr Whole ROTATION = 1;
constexpr Whole SURFACE = 2;
}  // namespace BOX::PBR

namespace BOX::BLOCKS {
struct Surface {
  GFX::Color color;
  Float factors[4];
};
}  // namespace BOX::BLOCKS
