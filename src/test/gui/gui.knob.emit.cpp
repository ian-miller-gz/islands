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
#include <island/gui/translate.hpp>
#include <test/gui/gui.internal.hpp>

namespace {

constexpr STRING::Hot CELL = "pan";

auto ink(GUI::Handle document) -> GFX::Color {
  Whole node = GUI::PARENTLESS;
  if (GUI::find(document, ::CELL, node) != 0) return GFX::COLORS::CLEAR;
  return GUI::STATE::documents[document].nodes[node].properties.track;
}

}  // namespace

auto KNOB::emit(GUI::Handle document) -> Status {
  String text;
  Status status = check("emit", GUI::TRANSLATE::emit(document, text) == 0);
  status |= check("track emit", text.find(" track=") != String::npos);
  const auto again =
    GUI::load("knob.emit", Vector<Byte>(text.begin(), text.end()));
  const auto stated = ::ink(document);
  status |= check("track trip", stated.a > 0.0f && ::ink(again) == stated);
  GUI::remove(again);
  return status;
}
