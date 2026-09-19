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
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/backend/pipelines.hpp>

namespace {

struct Clone {
  GFX::Bindings vertex;
  GFX::Bindings fragment;
  GFX::Handle handle;
};

struct Family {
  GFX::Pipeline description;
  Vector<Clone> clones;
};

Vector<Family> families;

constexpr GFX::Handle TAG = GFX::Handle(1) << 30;

auto realize(Family &family, const GFX::Draw &draw) -> GFX::Handle {
  for (const auto &clone : family.clones) {
    if (clone.vertex == draw.vertex && clone.fragment == draw.fragment)
      return clone.handle;
  }
  const auto handle = GFX::PIPELINES::create(family.description);
  family.clones.push_back({draw.vertex, draw.fragment, handle});
  return handle;
}

}  // namespace

auto GFX::PIPELINES::FAMILIES::create(const GFX::Pipeline &description)
  -> GFX::Handle {
  families.push_back({.description = description});
  return TAG | (families.size() - 1);
}

void GFX::PIPELINES::FAMILIES::resolve(Vector<GFX::Pass> &passes) {
  for (auto &pass : passes) {
    for (auto &draw : pass.draws) {
      if (draw.pipeline == GFX::NONE || !(draw.pipeline & TAG)) continue;
      draw.pipeline = realize(families[draw.pipeline & ~TAG], draw);
    }
  }
}

void GFX::PIPELINES::FAMILIES::destroy(GFX::Handle family) {
  auto &found = families[family & ~TAG];
  for (const auto &clone : found.clones) GFX::PIPELINES::destroy(clone.handle);
  found.clones.clear();
}

void GFX::PIPELINES::FAMILIES::close() { families.clear(); }
