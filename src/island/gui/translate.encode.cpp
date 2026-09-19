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
#include <cstdio>
#include <island/gui/translate.internal.hpp>

auto GUI::TRANSLATE::number(Float value) -> String {
  constexpr Whole WIDTH = 32;
  Byte text[WIDTH] = {};
  std::snprintf(text, sizeof(text), "%g", static_cast<double>(value));
  return text;
}

void GUI::TRANSLATE::hex(Float channel, String &text) {
  static constexpr STRING::Hot DIGITS = "0123456789abcdef";
  constexpr Float SCALE = 255.0f;
  constexpr Whole BASE = 16;
  const auto value = static_cast<Whole>(channel * SCALE + 0.5f);
  text += DIGITS[value / BASE];
  text += DIGITS[value % BASE];
}

auto GUI::TRANSLATE::shade(const GFX::Color &color) -> String {
  String text = "#";
  hex(color.r, text);
  hex(color.g, text);
  hex(color.b, text);
  hex(color.a, text);
  return text;
}
