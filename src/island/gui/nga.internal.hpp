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
#include <island/gui/nga.hpp>
#include <island/gui/gui.internal.hpp>

namespace GUI::NGA {

constexpr Whole SEGMENTS = 5;

constexpr Whole DASHES = 8;

constexpr Float THICKNESS = 2.0f;

constexpr Float PITCH = 3.0f;

constexpr Float STUB = 6.0f;

struct Span {
  Float x = 0.0f;
  Float y = 0.0f;
  Float w = 0.0f;
  Float h = 0.0f;
};

struct End {
  Position at;
  Whole rank = 0;
  Span box;
};

void route(End out, End in, Float thickness, Span *run);

auto rank(const Document &entry, Whole index) -> Whole;

void guide(Position out, Position in, Float thickness, Span *marks);

auto crosses(const Span &span, Position from, Position to) -> Flag;

auto resolved(Zoom zoom) -> Extent;

auto bounded(Handle document, Whole index, Pan pan) -> Pan;

void settle(Handle document, Whole index);

auto agree(const Document &entry, Whole source, Whole sink) -> Flag;

auto centre(Handle document, Whole index) -> Position;

auto scaled(Handle document, Whole index) -> Extent;

auto pending(const Document &entry) -> Whole;

auto banding(const Document &entry) -> Whole;

auto band(const Document &entry) -> Span;

auto caught(Handle document, Whole index, const Span &band) -> Flag;

}  // namespace GUI::NGA
