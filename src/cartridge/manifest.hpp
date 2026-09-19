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

#include <island/input.hpp>
#include <common.hpp>

namespace CARTRIDGE {

struct Requirement {
  String bundle;
  Flag ensure = false;
  Flag replace = false;
  Flag kill = false;
  Flag vital = false;
};

struct WindowSpec {
  String name;
  String title;
  Flag show = true;
};

struct Demand {
  String axis;
  Vector<String> values;
};

enum Coverage { SLIM, FULL };

struct Manifest {
  String name;
  String entry;
  String host;
  String assets;
  String cache;
  Whole version = 0;
  Flag overlay = false;
  Flag logs = false;
  Flag listen = false;
  Flag threads = false;
  Flag latch = false;
  Float budget = 0;
  Flag metrics = true;
  Flag fed = false;
  Flag decorated = true;
  Flag splash = false;
  Coverage font = SLIM;
  Whole width = 0;
  Whole height = 0;
  Whole console = INPUT::BINDINGS::keys["CONSOLES::DISPATCH::interpret"];
  Whole fullscreen = INPUT::BINDINGS::keys["WINDOW::fullscreen"];
  Vector<Requirement> requirements;
  Map<String, String> keys;
  Vector<String> web;
  Vector<String> server;
  Vector<WindowSpec> windows;
  Vector<Demand> demands;
  Vector<String> components;
};

namespace MANIFEST {
constexpr STRING::Hot NAME = "manifest.yaml";

auto load(const String &path, Manifest &manifest) -> Status;

auto read(const String &directory) -> Manifest;

auto entry(const String &directory, const Manifest &manifest) -> String;
}  // namespace MANIFEST

}  // namespace CARTRIDGE
