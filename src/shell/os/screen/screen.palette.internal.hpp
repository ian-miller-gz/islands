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

#include <island/graphics/color.hpp>
#include <common.hpp>

namespace SHELL::OS::SCREEN {

constexpr GFX::Color PALETTE[16] = {
  {0.05f, 0.06f, 0.08f, 1.0f}, {0.68f, 0.10f, 0.10f, 1.0f},
  {0.10f, 0.48f, 0.12f, 1.0f}, {0.60f, 0.45f, 0.05f, 1.0f},
  {0.13f, 0.30f, 0.70f, 1.0f}, {0.58f, 0.13f, 0.58f, 1.0f},
  {0.08f, 0.45f, 0.50f, 1.0f}, {0.20f, 0.23f, 0.28f, 1.0f},
  {0.45f, 0.48f, 0.52f, 1.0f}, {0.80f, 0.18f, 0.18f, 1.0f},
  {0.15f, 0.60f, 0.15f, 1.0f}, {0.72f, 0.55f, 0.06f, 1.0f},
  {0.20f, 0.40f, 0.85f, 1.0f}, {0.72f, 0.20f, 0.72f, 1.0f},
  {0.10f, 0.55f, 0.62f, 1.0f}, {0.08f, 0.09f, 0.11f, 1.0f}};
constexpr Whole BRIGHT = 8;
constexpr Whole PLAIN = 16;
constexpr GFX::Color FOREGROUND = PALETTE[7];
constexpr GFX::Color BACKGROUND = {0.90f, 0.93f, 0.96f, 0.97f};

struct Pen {
  GFX::Color fg = FOREGROUND;
  GFX::Color bg = BACKGROUND;
  Whole base = PLAIN;
  Flag bold = false;
  Flag reverse = false;
  Flag underline = false;
};

struct Cell {
  Whole glyph = ' ';
  GFX::Color fg = FOREGROUND;
  GFX::Color bg = BACKGROUND;
  Flag underline = false;
};

auto indexed(Whole index) -> GFX::Color;

auto extended(const Vector<Whole> &values, Whole &index, GFX::Color &color)
  -> Flag;

}  // namespace SHELL::OS::SCREEN
