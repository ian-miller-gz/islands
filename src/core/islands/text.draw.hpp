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
#pragma once
#include <island/graphics/sprites/sprites.hpp>

#include "text.hpp"

namespace ISLANDS::TEXT {

constexpr STRING::Hot MARKER = ">";

struct Style {
  GFX::Color ink = GFX::COLORS::WHITE;
  GFX::Color fill = {0.05f, 0.07f, 0.16f, 1.0f};
  GFX::Color border = {0.88f, 0.93f, 1.0f, 1.0f};
  Float edge = 1.0f;
  Float pad = 4.0f;
};

struct Dialog {
  Vector<String> pages;
  Whole page = 0;
};

inline auto more(const Dialog &dialog) -> Flag {
  return dialog.page + 1 < dialog.pages.size();
}

inline auto done(const Dialog &dialog) -> Flag {
  return dialog.page >= dialog.pages.size();
}

inline auto bake(const Font &font) -> GFX::SPRITES::Handle {
  Vector<GFX::SPRITES::ATLASES::Mask> cells;
  for (Whole glyph = 0; glyph < font.count; glyph += 1) {
    Vector<Byte> coverage(font.width * font.height, 0);
    for (Whole v = 0; v < font.height; v += 1)
      for (Whole u = 0; u < font.width; u += 1)
        coverage[v * font.width + u] =
          (font.rows[glyph * font.height + v] >> (font.width - 1 - u)) & 1;
    cells.push_back({std::move(coverage)});
  }
  cells.push_back({Vector<Byte>(font.width * font.height, 1)});
  return GFX::SPRITES::ATLASES::create({font.width, font.height}, cells);
}

inline void draw(
  GFX::SPRITES::Handle batch, GFX::SPRITES::Handle atlas, const Font &font,
  const String &text, Float x, Float y, GFX::Color color = GFX::COLORS::WHITE,
  Whole scale = 1) {
  const Float width = Float(font.width * scale),
              height = Float(font.height * scale);
  const Float step = Float(font.stride * scale);
  Whole index = 0;
  for (const char letter : text) {
    const Whole cell = column(font, letter);
    if (cell != NONE)
      GFX::SPRITES::cell(
        batch, x + Float(index) * step, y, width, height, atlas, cell, 0,
        color);
    index += 1;
  }
}

inline void paragraph(
  GFX::SPRITES::Handle batch, GFX::SPRITES::Handle atlas, const Font &font,
  const Vector<String> &lines, Float x, Float y,
  GFX::Color color = GFX::COLORS::WHITE, Whole scale = 1) {
  for (Whole row = 0; row < lines.size(); row += 1)
    draw(
      batch, atlas, font, lines[row], x, y + Float(row * font.line * scale),
      color, scale);
}

inline void box(
  GFX::SPRITES::Handle batch, GFX::SPRITES::Handle atlas, const Font &font,
  Float x, Float y, Float w, Float h, const Style &style = {}) {
  const auto panel = [&](Float px, Float py, Float pw, Float ph, GFX::Color c) {
    GFX::SPRITES::cell(batch, px, py, pw, ph, atlas, font.count, 0, c);
  };
  panel(x, y, w, h, style.fill);
  panel(x, y, w, style.edge, style.border);
  panel(x, y + h - style.edge, w, style.edge, style.border);
  panel(x, y + style.edge, style.edge, h - 2 * style.edge, style.border);
  panel(
    x + w - style.edge, y + style.edge, style.edge, h - 2 * style.edge,
    style.border);
}

inline void dialog(
  GFX::SPRITES::Handle batch, GFX::SPRITES::Handle atlas, const Font &font,
  const Dialog &dialog, Float x, Float y, Float w, Whole rows,
  const Style &style = {}) {
  if (done(dialog)) return;
  const Float height = 2 * style.pad + Float(rows * font.line);
  box(batch, atlas, font, x, y, w, height, style);
  const Whole budget = TEXT::budget(font, Whole(w - 2 * style.pad));
  const auto lines = wrap(dialog.pages[dialog.page], budget);
  const Whole shown = lines.size() < rows ? Whole(lines.size()) : rows;
  for (Whole row = 0; row < shown; row += 1)
    draw(
      batch, atlas, font, lines[row], x + style.pad,
      y + style.pad + Float(row * font.line), style.ink);
  if (more(dialog))
    draw(
      batch, atlas, font, MARKER, x + w - style.pad - Float(font.stride),
      y + height - style.pad - Float(font.height), style.ink);
}

}  // namespace ISLANDS::TEXT
