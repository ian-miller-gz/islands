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
#include <island/gui/gui.hpp>
#include <island/graphics/backend/passes.hpp>

namespace GUI::BACKEND {

void open();

void flush(Handle document, GFX::Pass &pass);

auto pick(Handle document, Position local) -> Whole;

auto extent(Handle document) -> Extent;

auto measured(Handle document, Whole node) -> Extent;

auto origin(Handle document, Whole node) -> Position;

auto caret(Handle document, Whole node, Float x) -> Whole;

auto defaults(STRING::Hot kind) -> Properties;

auto known(STRING::Hot kind) -> Flag;

auto row(STRING::Hot kind) -> Whole;

void forget(Handle document);

void close();

void reset();

auto writes() -> Whole;

}  // namespace GUI::BACKEND
