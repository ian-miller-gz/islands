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
#include <island/gui/gui.hpp>
#include <span>

namespace GUI::PARSE {

struct Ink {
  STRING::Hot key;
  GFX::Color Properties::*field;
};

extern const std::span<const Ink> INKS;

auto ink(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status;

auto ink(Properties &properties, const String &key, const String &value)
  -> Status;

auto inked(const String &key) -> Flag;

}  // namespace GUI::PARSE
