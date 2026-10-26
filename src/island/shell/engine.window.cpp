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
#include <cartridge.hpp>
#include <island/graphics/capture.hpp>
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

void report(::SHELL::Session &session) {
  session.print(std::format(
    "Window {}x{}, {}.", GFX::WINDOWS::MAIN::width, GFX::WINDOWS::MAIN::height,
    CARTRIDGE::decorated ? "decorated" : "borderless"));
}

void resize(::SHELL::Session &session) {
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
  session.print(std::format("Window size {}x{}.", width, height));
}

void fullscreen(::SHELL::Session &session) {
  const auto &arguments = session.arguments;
  Flag on = true;
  if (arguments.size() > 2) {
    if (arguments[2] == "off" || arguments[2] == "0")
      on = false;
    else if (arguments[2] != "on" && arguments[2] != "1")
      return session.print("Unknown argument: " + arguments[2]);
  }
  if (!WINDOW::GET::live()) return session.print("Window is not up.");
  WINDOW::fullscreen(on);
  session.print(on ? "Window fullscreen on." : "Window fullscreen off.");
}

void window(::SHELL::Session &session) {
  const String sub =
    session.arguments.size() > 1 ? session.arguments[1] : "report";
  if (sub == "report") return report(session);
  if (sub == "size") return resize(session);
  if (sub == "fullscreen") return fullscreen(session);
  session.print("usage: window [report | size <w>x<h> | fullscreen [on|off]]");
}

void screenshot(::SHELL::Session &session) {
  const auto &arguments = session.arguments;
  if (arguments.size() < 2) return session.print("usage: screenshot <path>");
  GFX::CAPTURE::path = arguments[1];
  GFX::CAPTURE::armed = true;
  session.print("Capturing the next frame to " + arguments[1] + ".");
}

}  // namespace

auto ISLAND::SHELL::commands() -> const Vector<::SHELL::Command> & {
  static const Vector<::SHELL::Command> table = {
    {"window",
     "reports or sets the window (window [report|size <w>x<h>|"
     "fullscreen [on|off]])",
     window},
    {"screenshot", "captures the next frame to a file (screenshot <path>)",
     screenshot}};
  return table;
}
