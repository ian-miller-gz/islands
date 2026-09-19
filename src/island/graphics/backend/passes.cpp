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
#include <utility>

static Vector<GFX::Pass> submitted;

void GFX::PASSES::submit(Vector<GFX::Pass> passes) {
  submitted = std::move(passes);
  GFX::PIPELINES::FAMILIES::resolve(submitted);
}

void GFX::PASSES::submit(GFX::Pass&& pass) {
  Vector<GFX::Pass> passes;
  passes.push_back(std::move(pass));
  submit(std::move(passes));
}

auto GFX::PASSES::list() -> const Vector<GFX::Pass>& { return submitted; }

static Vector<GFX::Dispatch> dispatched;

void GFX::DISPATCHES::submit(Vector<GFX::Dispatch> dispatches) {
  dispatched = std::move(dispatches);
}

auto GFX::DISPATCHES::list() -> const Vector<GFX::Dispatch>& {
  return dispatched;
}

auto GFX::PASSES::last() -> GFX::Pass& {
  if (submitted.empty() || submitted.back().target != GFX::NONE) {
    submitted.push_back({});
  }
  return submitted.back();
}
