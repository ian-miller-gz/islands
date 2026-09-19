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
#include <test/view/view.internal.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/windows.hpp>

int GFX::WINDOWS::MAIN::width = 0;
int GFX::WINDOWS::MAIN::height = 0;

static GFX::Handle handles = 0;
static GFX::SAMPLERS::Kind sampled = GFX::SAMPLERS::CLAMP;

auto VIEW::sampled() -> GFX::SAMPLERS::Kind { return ::sampled; }

auto GFX::TARGETS::create(Whole, Whole) -> GFX::Handle { return ++handles; }

void GFX::TARGETS::destroy(GFX::Handle) {}

auto GFX::SAMPLERS::create(Kind kind) -> GFX::Handle {
  ::sampled = kind;
  return ++handles;
}

void GFX::SAMPLERS::destroy(GFX::Handle) {}

auto GFX::PIPELINES::create(const GFX::Pipeline &) -> GFX::Handle {
  return ++handles;
}

void GFX::PIPELINES::destroy(GFX::Handle) {}
