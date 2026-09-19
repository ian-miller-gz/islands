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
#include <island/graphics/backend/state.hpp>
#include <island/run.hpp>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/graphics/text::"

namespace STATE = GFX::TEXT::STATE;

static void reset() {
  const auto frame = ISLAND::RUN::GET::frame();
  if (STATE::frame == frame) return;
  STATE::frame = frame;
  STATE::cursor = 0;
}

static auto kern(GFX::TEXT::Font &font, Whole prev, Whole code) -> Float {
  if (!prev) return 0;
  const auto key = GFX::TEXT::pair(prev, code);
  const auto found = font.kernings.find(key);
  if (found != font.kernings.end()) return found->second;
  return font.kernings[key] = GFX::TEXT::FACE::kern(prev, code, font.ratio);
}

static void compose(
  const String &text, Float pen, Float top, GFX::TEXT::Font &font,
  const GFX::Pass &pass, const GFX::Color &color) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "text");
  Whole prev = 0;
  const auto bytes = static_cast<Whole>(text.size());
  for (Whole index = 0; index < bytes;) {
    if (STATE::cursor >= GFX::TEXT::LIMIT * GFX::TEXT::CORNERS) {
      logger.debug("Glyph budget spent; dropping the run's tail.");
      return;
    }
    const auto code = UNICODE::decode(text, index);
    const auto *entry = GFX::TEXT::glyph(font, code);
    if (!entry) continue;
    pen += kern(font, prev, code);
    if (entry->width) GFX::TEXT::emit(*entry, pen, top, pass.viewport, color);
    pen += entry->advance;
    prev = code;
  }
}

auto GFX::TEXT::GET::scale() -> Float { return GFX::scale; }

auto GFX::TEXT::draw(
  GFX::Pass &pass, const String &text, Float x, Float y, Float scale,
  const GFX::Color &color) -> Whole {
  auto *font = directory(scale);
  if (!font) return 0;
  ::reset();
  const Whole first = STATE::cursor;
  ::compose(text, x * GFX::scale, y * GFX::scale, *font, pass, color);
  if (STATE::cursor == first) return 0;
  pass.draws.push_back(
    {.pipeline = STATE::pipeline,
     .fragment = {.samplers = {{STATE::texture, STATE::sampler}}},
     .vertices = STATE::buffer,
     .first = first,
     .count = STATE::cursor - first});
  return STATE::cursor - first;
}
