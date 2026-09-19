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
#include <common.hpp>

#include "text.font.hpp"

namespace ISLANDS::TEXT {

constexpr Whole NONE = static_cast<Whole>(-1);

struct Font {
  Whole width = 5;
  Whole height = 7;
  Whole stride = 6;
  Whole line = 8;
  char first = ' ';
  Whole count = 95;
  const Byte *rows = nullptr;
};

constexpr Font DEFAULT = {5, 7, 6, 8, ' ', 95, GLYPHS};

inline auto column(const Font &font, char letter) -> Whole {
  const Whole offset =
    static_cast<Whole>(static_cast<unsigned char>(letter)) -
    static_cast<Whole>(static_cast<unsigned char>(font.first));
  return letter < font.first || offset >= font.count ? NONE : offset;
}

inline auto measure(const Font &font, const String &text, Whole scale = 1)
  -> Whole {
  const Whole glyphs = static_cast<Whole>(text.size());
  return glyphs ? ((glyphs - 1) * font.stride + font.width) * scale : 0;
}

inline auto budget(const Font &font, Whole pixels) -> Whole {
  return pixels < font.width ? 0 : (pixels - font.width) / font.stride + 1;
}

inline auto wrap(const String &text, Whole budget) -> Vector<String> {
  Vector<String> lines;
  String line, word;
  auto fold = [&] {
    if (word.empty()) return;
    if (line.empty())
      line = word;
    else if (line.size() + 1 + word.size() <= budget)
      line += ' ', line += word;
    else
      lines.push_back(line), line = word;
    word.clear();
  };
  for (const char letter : text)
    if (letter == ' ')
      fold();
    else
      word += letter;
  fold();
  if (!line.empty()) lines.push_back(line);
  return lines;
}

}  // namespace ISLANDS::TEXT
