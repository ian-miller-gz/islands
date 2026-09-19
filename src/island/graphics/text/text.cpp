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
#include <island/graphics/text/text.internal.hpp>
#include <island/graphics/text/text.outline.internal.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/state.hpp>

static const Vector<Whole> LAYOUT = {2, 2, 4};

Map<Whole, GFX::TEXT::Font> GFX::TEXT::STATE::sizings;
Vector<Byte> GFX::TEXT::STATE::pixels;
GFX::TEXT::Shelf GFX::TEXT::STATE::shelf;
GFX::TEXT::Band GFX::TEXT::STATE::owed;
GFX::Handle GFX::TEXT::STATE::texture = GFX::NONE;
GFX::Handle GFX::TEXT::STATE::sampler = GFX::NONE;
GFX::Handle GFX::TEXT::STATE::pipeline = GFX::NONE;
GFX::Handle GFX::TEXT::STATE::buffer = GFX::NONE;
Whole GFX::TEXT::STATE::cursor = 0;
Whole GFX::TEXT::STATE::frame = 0;
Flag GFX::TEXT::STATE::ready = false;
Flag GFX::TEXT::STATE::refused = false;

namespace STATE = GFX::TEXT::STATE;

static void build() {
  STATE::sampler = GFX::SAMPLERS::create(GFX::SAMPLERS::CLAMP);
  STATE::pipeline = GFX::PIPELINES::create(
    {.vertex = {.path = String(GFX::TEXT::SHADERS) + ".vertex"},
     .fragment =
       {.path = String(GFX::TEXT::SHADERS) + ".fragment", .samplers = 1},
     .attributes = LAYOUT});
  STATE::buffer = GFX::BUFFERS::create(
    GFX::TEXT::LIMIT * GFX::TEXT::CORNERS * GFX::TEXT::WIDTHS * sizeof(Float),
    GFX::BUFFERS::VERTICES);
  STATE::texture = GFX::TEXTURES::create(GFX::TEXT::PAGE, GFX::TEXT::PAGE);
  GFX::TEXT::reserve();
}

auto GFX::TEXT::ready() -> Flag {
  if (STATE::ready) return true;
  if (STATE::refused) return false;
  if (FACE::open()) {
    STATE::refused = true;
    return false;
  }
  ::build();
  STATE::ready = true;
  return true;
}

auto GFX::TEXT::directory(Float scale) -> GFX::TEXT::Font * {
  if (!GFX::TEXT::ready()) return nullptr;
  return GFX::TEXT::sizing(scale * GFX::TEXT::EM * GFX::scale);
}

auto GFX::TEXT::GET::line(Float scale) -> Float {
  const auto *font = directory(scale);
  return font ? font->line / GFX::scale : 0;
}

auto GFX::TEXT::GET::advance(Float scale) -> Float {
  const auto *font = directory(scale);
  return font ? font->advance / GFX::scale : 0;
}

auto GFX::TEXT::GET::font(Float scale) -> const GFX::TEXT::Font & {
  static const Font empty;
  const auto *font = directory(scale);
  return font ? *font : empty;
}

auto GFX::TEXT::GET::glyph(Whole code, Float scale) -> const Glyph * {
  auto *font = directory(scale);
  if (!font) return nullptr;
  return GFX::TEXT::glyph(*font, code);
}

auto GFX::TEXT::GET::sampling() -> GFX::Sampling {
  if (!GFX::TEXT::ready()) return {};
  return {STATE::texture, STATE::sampler};
}

void GFX::TEXT::close() {
  STATE::sizings.clear();
  STATE::pixels.clear();
  STATE::shelf = {};
  STATE::owed = {};
  STATE::texture = STATE::sampler = GFX::NONE;
  STATE::pipeline = STATE::buffer = GFX::NONE;
  STATE::cursor = 0;
  STATE::frame = 0;
  STATE::ready = false;
  STATE::refused = false;
  FACE::close();
  OUTLINE::close();
}
