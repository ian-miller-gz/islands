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
#include <cmath>
#define LOGGER_CATEGORY "~/island/graphics/text::"

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

namespace {
Vector<Byte> bytes;
stbtt_fontinfo info;
Flag opened = false;
}  // namespace

auto GFX::TEXT::FACE::open() -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "text");
  if (opened) return 0;
  if (IO::read(FACE::PATH, bytes) != 0) {
    logger.error("No text face to rasterize from: %s.", FACE::PATH.c_str());
    return 1;
  }
  const auto *data = reinterpret_cast<const unsigned char *>(bytes.data());
  if (bytes.empty() || !stbtt_InitFont(&info, data, 0)) {
    logger.error("The text face refused to parse: %s.", FACE::PATH.c_str());
    bytes.clear();
    return 1;
  }
  opened = true;
  return 0;
}

auto GFX::TEXT::FACE::ratio(Float pixels) -> Float {
  return opened ? stbtt_ScaleForPixelHeight(&info, pixels) : 0.0f;
}

void GFX::TEXT::FACE::metrics(Float ratio, Font &font) {
  int ascent = 0, descent = 0, gap = 0, advance = 0, bearing = 0;
  stbtt_GetFontVMetrics(&info, &ascent, &descent, &gap);
  stbtt_GetCodepointHMetrics(&info, REFERENCE, &advance, &bearing);
  font.ratio = ratio;
  font.ascent = std::round(static_cast<Float>(ascent) * ratio);
  font.line = std::round(static_cast<Float>(ascent - descent) * ratio);
  font.advance = std::round(static_cast<Float>(advance) * ratio);
}

auto GFX::TEXT::FACE::covers(Whole code) -> Flag {
  return opened && stbtt_FindGlyphIndex(&info, static_cast<int>(code)) != 0;
}

auto GFX::TEXT::FACE::raster(
  Whole code, const Font &font, Glyph &glyph, Vector<Byte> &coverage) -> Flag {
  const auto point = static_cast<int>(code);
  int x0 = 0, y0 = 0, x1 = 0, y1 = 0, advance = 0, bearing = 0;
  stbtt_GetCodepointBitmapBox(
    &info, point, font.ratio, font.ratio, &x0, &y0, &x1, &y1);
  stbtt_GetCodepointHMetrics(&info, point, &advance, &bearing);
  glyph.width = static_cast<Float>(x1 - x0);
  glyph.height = static_cast<Float>(y1 - y0);
  glyph.bearing[0] = static_cast<Float>(x0);
  glyph.bearing[1] = font.ascent + static_cast<Float>(y0);
  glyph.advance = font.advance;
  if (glyph.width <= 0 || glyph.height <= 0) return true;
  const auto wide = static_cast<int>(glyph.width);
  const auto tall = static_cast<int>(glyph.height);
  coverage.assign(static_cast<Whole>(wide * tall), 0);
  stbtt_MakeCodepointBitmap(
    &info, reinterpret_cast<unsigned char *>(coverage.data()), wide, tall, wide,
    font.ratio, font.ratio, point);
  return true;
}

auto GFX::TEXT::FACE::kern(Whole first, Whole second, Float ratio) -> Float {
  const auto raw = stbtt_GetCodepointKernAdvance(
    &info, static_cast<int>(first), static_cast<int>(second));
  return static_cast<Float>(raw) * ratio;
}

void GFX::TEXT::FACE::close() {
  bytes.clear();
  opened = false;
}
