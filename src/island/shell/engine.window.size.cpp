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
#include <island/graphics/windows.hpp>
#include <island/shell/engine.internal.hpp>
#include <island/window/backend.hpp>
#include <charconv>
#include <format>

namespace {

auto parse(const String &text, int &out) -> Flag {
  auto [end, code] =
    std::from_chars(text.data(), text.data() + text.size(), out);
  return code == std::errc() && end == text.data() + text.size() && out > 0;
}

}  // namespace

void ISLAND::SHELL::resize(::SHELL::Session &session) {
  const auto &arguments = session.arguments;
  if (arguments.size() < 3) return session.print("usage: window size <w>x<h>");
  int width = 0, height = 0;
  const auto cut = arguments[2].find('x');
  if (
    cut == String::npos || !parse(arguments[2].substr(0, cut), width) ||
    !parse(arguments[2].substr(cut + 1), height))
    return session.print("Unknown size: " + arguments[2]);
  GFX::WINDOWS::MAIN::width = width;
  GFX::WINDOWS::MAIN::height = height;
  if (WINDOW::GET::live())
    WINDOW::resize(static_cast<Whole>(width), static_cast<Whole>(height));
  session.print(std::format("Window size {}x{}.", width, height));
}
