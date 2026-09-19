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
#include <logger.hpp>
#include <algorithm>
#define LOGGER_CATEGORY "~/island/graphics/text::"

namespace STATE = GFX::TEXT::STATE;

void GFX::TEXT::reserve() {
  STATE::pixels.assign(PAGE * PAGE * CHANNELS, 0);
  STATE::shelf = {};
  STATE::owed = {};
  if (STATE::texture != GFX::NONE)
    GFX::TEXTURES::write(STATE::texture, STATE::pixels.data());
}

static void owe(Whole top, Whole rows) {
  auto &owed = STATE::owed;
  if (owed.bottom == owed.top) {
    owed = {top, top + rows};
    return;
  }
  owed.top = std::min(owed.top, top);
  owed.bottom = std::max(owed.bottom, top + rows);
}

static auto room(Whole wide, Whole tall, Whole &x, Whole &y) -> Flag {
  auto &shelf = STATE::shelf;
  if (shelf.x + wide > GFX::TEXT::PAGE) {
    shelf.y += shelf.tall + GFX::TEXT::PADDING;
    shelf.x = 0;
    shelf.tall = 0;
  }
  if (shelf.y + tall > GFX::TEXT::PAGE) return false;
  x = shelf.x;
  y = shelf.y;
  shelf.x += wide + GFX::TEXT::PADDING;
  shelf.tall = std::max(shelf.tall, tall);
  return true;
}

auto GFX::TEXT::pack(const Vector<Byte> &coverage, Glyph &glyph) -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "text");
  const auto wide = static_cast<Whole>(glyph.width);
  const auto tall = static_cast<Whole>(glyph.height);
  Whole x = 0, y = 0;
  if (!room(wide, tall, x, y)) {
    logger.debug("The glyph page is full; dropping a glyph.");
    return false;
  }
  for (Whole row = 0; row < tall; row += 1)
    for (Whole col = 0; col < wide; col += 1) {
      auto *texel =
        STATE::pixels.data() + ((y + row) * PAGE + x + col) * CHANNELS;
      texel[0] = texel[1] = texel[2] = static_cast<Byte>(0xFF);
      texel[3] = coverage[row * wide + col];
    }
  glyph.x = static_cast<Float>(x);
  glyph.y = static_cast<Float>(y);
  ::owe(y, tall);
  return true;
}

void GFX::TEXT::flush() {
  auto &owed = STATE::owed;
  if (owed.bottom == owed.top || STATE::texture == GFX::NONE) return;
  GFX::TEXTURES::write(
    STATE::texture, STATE::pixels.data() + owed.top * PAGE * CHANNELS, 0,
    owed.top, PAGE, owed.bottom - owed.top);
  owed = {};
}
