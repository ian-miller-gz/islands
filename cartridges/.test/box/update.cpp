// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/model/model.pipeline.hpp>
#include <island/graphics/windows.hpp>

static void draw(const GFX::MODEL::Mesh &mesh) {
  auto &draws = BOX::pass.draws;
  draws.push_back(
    {.pipeline = BOX::PIPELINES::pbr,
     .vertices = mesh.vertices,
     .indices = mesh.indices,
     .count = mesh.count});
  if (mesh.material.texture == GFX::NONE) return;
  draws.push_back(
    {.pipeline = BOX::PIPELINES::unlit,
     .fragment = {.samplers = {{mesh.material.texture, BOX::sampler}}},
     .vertices = mesh.vertices,
     .indices = mesh.indices,
     .count = mesh.count});
}

void BOX::update() {
  pass.viewport = GFX::WINDOWS::MAIN::viewport();
  pass.draws.clear();
  if (!scene.meshes.empty()) {
    const auto lit = GET::camera(pass.viewport.w / pass.viewport.h, -SPREAD);
    GFX::PIPELINES::write(PIPELINES::pbr, PBR::CAMERA, &lit);
    const auto flat = GET::camera(pass.viewport.w / pass.viewport.h, SPREAD);
    GFX::PIPELINES::write(PIPELINES::unlit, GFX::MODEL::UNLIT::CAMERA, &flat);
    for (const auto &mesh : scene.meshes) draw(mesh);
  }
  GFX::PASSES::submit({pass});
}
