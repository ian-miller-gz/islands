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
#include <island/graphics/vsync.hpp>
#include <island/run.hpp>
#include <island/shell/engine.internal.hpp>
#include <island/state.hpp>

namespace {

void summary(::SHELL::Session &session) {
  session.print(ISLAND::RUN::report());
}

void quit(::SHELL::Session &) { ISLAND::STATE::terminate = true; }

void reload(::SHELL::Session &session) {
  if (!CARTRIDGE::DYNAMIC)
    return session.print("Reload requires dynamic delivery.");
  if (!CARTRIDGE::configured()) return session.print("No cartridge to reload.");
  ISLAND::STATE::reload = true;
}

void vsync(::SHELL::Session &session) {
  const auto &arguments = session.arguments;
  if (arguments.size() < 2)
    return session.print(GFX::VSYNC::on ? "Vsync is on." : "Vsync is off.");
  if (arguments[1] == "--help" || arguments[1] == "-help")
    return session.print("usage: " + arguments[0] + " [0|1]");
  if (arguments[1] != "0" && arguments[1] != "1")
    return session.print("Unknown argument: " + arguments[1]);
  GFX::VSYNC::on = arguments[1] == "1";
  session.print(GFX::VSYNC::on ? "Vsync on." : "Vsync off.");
}

struct Install {
  Install() {
    ::SHELL::ENGINE::host = {
      summary,
      quit,
      reload,
      ISLAND::SHELL::version,
      vsync,
      &ISLAND::SHELL::settings(),
      ISLAND::SHELL::status,
      &ISLAND::SHELL::commands(),
      ISLAND::SHELL::debug()};
  }
} install;

}  // namespace
