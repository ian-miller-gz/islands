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
#include <island/graphics/text/text.outline.internal.hpp>
#include <island/run.hpp>
#include <cstring>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/graphics/text::"

namespace STATE = GFX::TEXT::OUTLINE::STATE;
namespace OUTLINE = GFX::TEXT::OUTLINE;

static void reset() {
  const auto frame = ISLAND::RUN::GET::frame();
  if (STATE::frame == frame) return;
  STATE::frame = frame;
  STATE::cursor = 0;
}

static auto kern(Whole prev, Whole code) -> Float {
  const auto found = STATE::face.kernings.find(GFX::TEXT::pair(prev, code));
  return found == STATE::face.kernings.end() ? 0.0f : found->second;
}

static auto glyph(Whole code) -> const OUTLINE::Glyph * {
  const auto &glyphs = STATE::face.glyphs;
  auto found = glyphs.find(code);
  if (found == glyphs.end() && !UNICODE::control(code))
    found = glyphs.find(UNICODE::REPLACEMENT);
  return found == glyphs.end() ? nullptr : &found->second;
}

static void compose(const String &text, const GFX::Color &color) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "outline");
  Float pen = 0;
  Whole prev = 0;
  const auto bytes = static_cast<Whole>(text.size());
  for (Whole index = 0; index < bytes;) {
    if (STATE::cursor >= OUTLINE::LIMIT * OUTLINE::CORNERS) {
      logger.debug("Glyph budget spent; dropping the run's tail.");
      return;
    }
    const auto code = UNICODE::decode(text, index);
    const auto *entry = ::glyph(code);
    if (!entry) continue;
    pen += ::kern(prev, code);
    if (entry->high[0] > entry->low[0])
      GFX::TEXT::OUTLINE::emit(*entry, pen, color);
    pen += entry->advance;
    prev = code;
  }
}

void GFX::TEXT::draw(
  GFX::Pass &pass, const String &text, const Float matrix[16],
  const GFX::Color &color) {
  if (!OUTLINE::ready()) return;
  ::reset();
  const Whole first = OUTLINE::STATE::cursor;
  ::compose(text, color);
  if (OUTLINE::STATE::cursor == first) return;
  OUTLINE::Camera camera;
  std::memcpy(camera.matrix, matrix, sizeof camera.matrix);
  camera.viewport[0] = pass.viewport.w;
  camera.viewport[1] = pass.viewport.h;
  pass.draws.push_back(
    {.pipeline = OUTLINE::STATE::pipeline,
     .uniforms = {GFX::pack(camera)},
     .fragment =
       {.samplers =
          {{OUTLINE::STATE::curves, OUTLINE::STATE::sampler},
           {OUTLINE::STATE::strips, OUTLINE::STATE::sampler}}},
     .vertices = OUTLINE::STATE::buffer,
     .first = first,
     .count = OUTLINE::STATE::cursor - first});
}
