// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/meshes/meshes.hpp>

#include <cstring>

#include "state.hpp"

static auto ground(const String &home, const String &name) -> GFX::Handle {
  return GFX::PIPELINES::create(
    {.vertex = {.path = home + "idle.vertex"},
     .fragment =
       {.path = home + name, .uniforms = {sizeof(LAUNCHER::IDLE::Field)}},
     .attributes = {2, 2},
     .blend = false});
}

void LAUNCHER::IDLE::create() {
  const String home = CARTRIDGE::GET::asset("shaders/");
  pipeline = ::ground(home, "idle.fragment");
  night = ::ground(home, "shade.fragment");
  const auto corners = GFX::MESHES::fullscreen();
  quad = GFX::BUFFERS::create(
    corners.size() * sizeof(Float), GFX::BUFFERS::VERTICES);
  std::memcpy(
    GFX::BUFFERS::GET::data(quad), corners.data(),
    corners.size() * sizeof(Float));
}

void LAUNCHER::IDLE::paint(GFX::Pass &pass) {
  const auto dress = GUI::GET::theme() == GUI::DARK ? night : pipeline;
  if (dress == GFX::NONE) return;
  const Field field = {
    .time = CLOCK::GET::elapsed(),
    .aspect = pass.viewport.h > 0 ? pass.viewport.w / pass.viewport.h : 1.0f};
  pass.draws.push_back(
    {.pipeline = dress,
     .uniforms = {GFX::pack(field)},
     .vertices = quad,
     .count = GFX::MESHES::VERTICES});
}
