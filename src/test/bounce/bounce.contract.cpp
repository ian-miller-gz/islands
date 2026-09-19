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
#include "bounce.internal.hpp"

namespace {

auto described(const AUDIO::PLUGIN::Plug &surface, void *instance) -> Flag {
  const Whole count = surface.parameters(instance);
  if (count == 0) return false;
  for (Whole index = 0; index < count; ++index) {
    AUDIO::PLUGIN::Control row;
    if (!surface.control(instance, index, row)) return false;
    if (row.most <= row.least) return false;
    if (row.resting < row.least || row.resting > row.most) return false;
    if (surface.name(instance, index).empty()) return false;
  }
  return true;
}

auto rested(const AUDIO::PLUGIN::Plug &surface, void *instance) -> Flag {
  for (Whole index = 0; index < surface.parameters(instance); ++index) {
    AUDIO::PLUGIN::Control row;
    if (!surface.control(instance, index, row)) return false;
    if (surface.held(instance, index) != row.resting) return false;
  }
  return true;
}

void dial(
  const AUDIO::PLUGIN::Plug &surface, void *instance, Whole index,
  Float value) {
  AUDIO::PLUGIN::Sample block[BOUNCE::BLOCK];
  AUDIO::PLUGIN::Sample *outputs[BOUNCE::CHANNELS] = {block};
  const AUDIO::PLUGIN::Event edit = {
    AUDIO::PLUGIN::Event::CONTROLLER, 0, index, value};
  surface.render(instance, outputs, BOUNCE::BLOCK, &edit, 1);
}

auto returned(const AUDIO::PLUGIN::Plug &surface, void *instance) -> Flag {
  const Whole count = surface.parameters(instance);
  AUDIO::PLUGIN::Control row;
  for (Whole index = 0; index < count; ++index) {
    if (!surface.control(instance, index, row) || row.steps != 0) continue;
    const Float inside = (row.least + row.most) / 2;
    dial(surface, instance, index, inside);
    if (surface.held(instance, index) != inside) return false;
    dial(surface, instance, index, row.most * 2 + 1);
    return surface.held(instance, index) == row.most;
  }
  return false;
}

auto guarded(const AUDIO::PLUGIN::Plug &surface, void *instance) -> Flag {
  AUDIO::PLUGIN::Control row;
  const Whole past = surface.parameters(instance);
  return surface.parameters(nullptr) == 0 &&
         !surface.control(nullptr, 0, row) &&
         surface.name(nullptr, 0).empty() && surface.held(nullptr, 0) == 0 &&
         !surface.control(instance, past, row) &&
         surface.name(instance, past).empty() &&
         surface.reading(instance, past).empty();
}

}  // namespace

auto BOUNCE::contract(const AUDIO::PLUGIN::Plug &surface) -> Status {
  void *instance = surface.create(RATE, CHANNELS);
  Status status =
    check("every row describes itself", described(surface, instance));
  status |=
    check("a fresh instance rests where it says", rested(surface, instance));
  status |=
    check("an edit returns in its own units", returned(surface, instance));
  status |=
    check("the hooks answer before a host knows", guarded(surface, instance));
  surface.destroy(instance);
  return status;
}
