// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/model/model.pipeline.hpp>

static void unlit(const GFX::MODEL::Material &material) {
  BOX::PIPELINES::unlit = GFX::MODEL::pipeline();
  GFX::PIPELINES::write(
    BOX::PIPELINES::unlit, GFX::MODEL::UNLIT::COLOR, &material.color);
}

static void pbr(const GFX::MODEL::Material &material) {
  BOX::PIPELINES::pbr = GFX::PIPELINES::create(
    {.vertex =
       {.path = CARTRIDGE::GET::asset("shaders/pbr.vertex"),
        .uniforms =
          {sizeof(GFX::SHADERS::Camera), sizeof(GFX::SHADERS::Camera)}},
     .fragment =
       {.path = CARTRIDGE::GET::asset("shaders/pbr.fragment"),
        .uniforms = {sizeof(BOX::BLOCKS::Surface)}},
     .attributes = GFX::MODEL::LAYOUT::ATTRIBUTES,
     .blend = false,
     .depth = true});
  const BOX::BLOCKS::Surface surface = {
    .color = material.color,
    .factors = {material.metallic, material.roughness, 0.0f, 0.0f}};
  GFX::PIPELINES::write(BOX::PIPELINES::pbr, BOX::PBR::SURFACE, &surface);
  const auto turn = BOX::GET::rotation();
  GFX::PIPELINES::write(BOX::PIPELINES::pbr, BOX::PBR::ROTATION, &turn);
}

void BOX::create() {
  const auto &material = scene.meshes.front().material;
  ::unlit(material);
  ::pbr(material);
  sampler = GFX::SAMPLERS::create(GFX::SAMPLERS::REPEAT);
}
