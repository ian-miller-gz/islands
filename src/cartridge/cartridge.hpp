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

#include <cartridge/defer.hpp>
#include <cartridge/interface.hpp>
#include <cartridge/manifest.hpp>
#include <cartridge/selection.hpp>
#include <common.hpp>

namespace CARTRIDGE {

constexpr Flag DYNAMIC = SR_CARTRIDGE == SR_NONE;

extern String path;
constexpr STRING::Hot NONE = "none";

extern String assets;

extern String cache;

extern Map<String, String> keys;

extern Vector<Requirement> requirements;

extern Flag overlay;

extern Flag logs;

extern Flag listen;

extern Flag threads;

extern Flag latch;

extern Float budget;

extern Flag metrics;

extern Flag fed;

extern Flag decorated;

extern Flag splash;

extern Whole width;
extern Whole height;

extern Coverage font;

extern Whole fullscreen;

extern Vector<String> web;

extern Vector<String> server;

extern Vector<String> components;

struct Panel {
  String name;
  String title;
  void (*prerender)() = nullptr;
  Flag show = true;
};

extern Vector<Panel> panels;

struct Preset {
  String name;
  Flag show;
};

extern Vector<Preset> presets;

void preset(const String &name, Flag show);

extern Vector<Closure> deferred;

extern Status status;

extern Flag finished;

extern Flag (*shadowed)(STRING::Hot name);

auto load() -> Status;
void unload();
auto configured() -> Flag;

void initialize();
void frame();
void serve();
void drain();
void close();
}  // namespace CARTRIDGE

namespace CARTRIDGE::GET {
auto entry() -> const Interface *;
auto bundle() -> String;
auto title() -> String;
auto windows() -> const Vector<Window> &;
auto panels() -> const Vector<Panel> &;
auto commands() -> const Vector<SHELL::Command> &;
}  // namespace CARTRIDGE::GET
