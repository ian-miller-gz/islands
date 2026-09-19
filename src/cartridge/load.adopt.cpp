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
#include <cartridge/cartridge.hpp>
#include <cartridge/load.internal.hpp>
#include <console.hpp>

static void seed(const Vector<CARTRIDGE::WindowSpec> &windows) {
  using namespace CARTRIDGE;
  for (const auto &spec : windows)
    panels.push_back(
      {spec.name, spec.title.empty() ? spec.name : spec.title, nullptr,
       spec.show});
  for (const Preset &recorded : presets)
    for (Panel &panel : panels)
      if (panel.name == recorded.name) panel.show = recorded.show;
}

void CARTRIDGE::adopt(const Manifest &manifest, const String &root) {
  if (!manifest.assets.empty()) assets = root + manifest.assets + "/";
  if (!manifest.cache.empty()) cache = root + manifest.cache + "/";
  title = manifest.name;
  keys = manifest.keys;
  requirements = manifest.requirements;
  overlay = manifest.overlay;
  logs = manifest.logs;
  listen = manifest.listen;
  threads = manifest.threads;
  latch = manifest.latch;
  budget = manifest.budget;
  fed = manifest.fed;
  metrics = manifest.metrics;
  decorated = manifest.decorated;
  splash = manifest.splash;
  font = manifest.font;
  width = manifest.width;
  height = manifest.height;
  web = manifest.web;
  server = manifest.server;
  components = manifest.components;
  ::seed(manifest.windows);
  CONSOLES::DISPATCH::hotkey = manifest.console;
  fullscreen = manifest.fullscreen;
}
