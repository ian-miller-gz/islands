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
#include <console.hpp>
#include <island/graphics/vsync.hpp>
#include <island/graphics/windows.hpp>
#include <island/shell/engine.internal.hpp>
#include <format>
#include <metrics.hpp>

namespace {

auto flag(Flag on) -> String { return on ? "on" : "off"; }

auto binding(Whole ordinal) -> String {
  return ordinal ? "f" + std::to_string(ordinal) : "unbound";
}

auto extent() -> String {
  return std::format(
    "{}x{}", GFX::WINDOWS::MAIN::width, GFX::WINDOWS::MAIN::height);
}

auto coverage() -> String {
  return CARTRIDGE::font == CARTRIDGE::FULL ? "full" : "slim";
}

auto parse(const String &value, Flag &target) -> Flag {
  if (value == "on" || value == "1") return target = true, true;
  if (value == "off" || value == "0") return target = false, true;
  return false;
}

auto metrics(const String &value) -> Flag {
  if (!parse(value, CARTRIDGE::metrics)) return false;
  METRICS::adopt(CARTRIDGE::metrics);
  return true;
}

}  // namespace

auto ISLAND::SHELL::settings() -> const Vector<::SHELL::ENGINE::Setting> & {
  static const Vector<::SHELL::ENGINE::Setting> table = {
    {"overlay", [] { return flag(CARTRIDGE::overlay); }, nullptr},
    {"logs", LOGS::level, LOGS::set},
    {"metrics", [] { return flag(CARTRIDGE::metrics); }, metrics},
    {"vsync", [] { return flag(GFX::VSYNC::on); },
     [](const String &value) { return parse(value, GFX::VSYNC::on); }},
    {"fullscreen", [] { return binding(CARTRIDGE::fullscreen); }, nullptr},
    {"decorated", [] { return flag(CARTRIDGE::decorated); }, nullptr},
    {"splash", [] { return flag(CARTRIDGE::splash); }, nullptr},
    {"size", extent, nullptr},
    {"console", [] { return binding(CONSOLES::DISPATCH::hotkey); }, nullptr},
    {"font", coverage, nullptr}};
  return table;
}
