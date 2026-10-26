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
#include <island/graphics/text/text.fallback.internal.hpp>
#include <logger.hpp>
#include <cmath>
#include <utility>
#define LOGGER_CATEGORY "~/island/graphics/text::"
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

namespace {
Vector<Byte> bytes;
stbtt_fontinfo info;
Flag tried = false, opened = false;
auto ready() -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "text");
  if (std::exchange(tried, true)) return opened;
  const String &path = GFX::TEXT::FACE::FALLBACK;
  if (IO::read(path, bytes) != 0 || bytes.empty()) {
    logger.info("No fallback face: %s", path.c_str());
    return false;
  }
  const auto *data = reinterpret_cast<const unsigned char *>(bytes.data());
  opened = stbtt_InitFont(&info, data, 0) != 0;
  if (!opened) logger.error("Fallback face refused: %s", path.c_str());
  return opened;
}

auto carries(Whole code) -> Flag {
  return ready() && stbtt_FindGlyphIndex(&info, static_cast<int>(code)) != 0;
}
auto raster(
  int point, const GFX::TEXT::Font &font, GFX::TEXT::Glyph &glyph,
  Vector<Byte> &coverage) -> Flag {
  const Float ratio = stbtt_ScaleForMappingEmToPixels(&info, font.line);
  int x0 = 0, y0 = 0, x1 = 0, y1 = 0, advance = 0, bearing = 0;
  stbtt_GetCodepointBitmapBox(&info, point, ratio, ratio, &x0, &y0, &x1, &y1);
  stbtt_GetCodepointHMetrics(&info, point, &advance, &bearing);
  glyph.width = static_cast<Float>(x1 - x0);
  glyph.height = static_cast<Float>(y1 - y0);
  glyph.bearing[0] = static_cast<Float>(x0);
  glyph.bearing[1] = font.ascent + static_cast<Float>(y0);
  glyph.advance = std::round(static_cast<Float>(advance) * ratio);
  if (glyph.width <= 0 || glyph.height <= 0) return true;
  const int wide = int(glyph.width), tall = int(glyph.height);
  coverage.assign(static_cast<Whole>(wide * tall), 0);
  stbtt_MakeCodepointBitmap(
    &info, reinterpret_cast<unsigned char *>(coverage.data()), wide, tall, wide,
    ratio, ratio, point);
  return true;
}
}  // namespace

auto GFX::TEXT::FACE::covers(Whole code) -> Flag {
  return PRIMARY::covers(code) || ::carries(code);
}

auto GFX::TEXT::FACE::raster(
  Whole code, const Font &font, Glyph &glyph, Vector<Byte> &coverage) -> Flag {
  if (PRIMARY::covers(code) || !::carries(code))
    return PRIMARY::raster(code, font, glyph, coverage);
  return ::raster(static_cast<int>(code), font, glyph, coverage);
}
