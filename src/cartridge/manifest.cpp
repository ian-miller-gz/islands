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
#include <cartridge/interface.hpp>
#include <cartridge/manifest.hpp>
#include <cartridge/manifest.internal.hpp>
#include <cstdlib>
#include <logger.hpp>
#include <string>
#define LOGGER_CATEGORY "~/cartridge::"

static auto ordinal(const String &value, Whole fallback) -> Whole {
  static constexpr STRING::Hot DIGITS = "0123456789";
  if (value == "true" || value == "1") return fallback;
  if (
    value.size() > 1 && (value[0] == 'f' || value[0] == 'F') &&
    value.find_first_not_of(DIGITS, 1) == String::npos)
    return std::strtoul(value.c_str() + 1, nullptr, 10);
  return 0;
}

static void extent(const String &value, CARTRIDGE::Manifest &manifest) {
  const auto by = value.find_first_of("xX");
  if (by == String::npos) return;
  const Whole width = std::strtoul(value.c_str(), nullptr, 10);
  const Whole height = std::strtoul(value.c_str() + by + 1, nullptr, 10);
  if (!width || !height) return;
  manifest.width = width;
  manifest.height = height;
}

static void pace(const String &value, CARTRIDGE::Manifest &manifest) {
  if (value == "auto") {
    manifest.latch = true;
    return;
  }
  const Float stated = std::strtof(value.c_str(), nullptr);
  if (stated <= 0) return;
  manifest.latch = true;
  manifest.budget = stated;
}

auto CARTRIDGE::MANIFEST::reserved(const String &key) -> Flag {
  return key == "name" || key == "entry" || key == "assets" || key == "cache" ||
         key == "version" || key == "overlay" || key == "logs" ||
         key == "listen" || key == "threads" || key == "metrics" ||
         key == "latch" || key == "stdin" || key == "web" || key == "server" ||
         key == "console" || key == "fullscreen" || key == "decorated" ||
         key == "splash" || key == "size" || key == "font" ||
         key == "requires" || key == "host";
}

auto CARTRIDGE::MANIFEST::assign(
  CARTRIDGE::Manifest &manifest, const String &key,
  const String &value) -> Status {
  if (key == "name") manifest.name = value;
  if (key == "entry") manifest.entry = value;
  if (key == "host") manifest.host = value;
  if (key == "assets") manifest.assets = value;
  if (key == "cache") manifest.cache = value;
  if (key == "version")
    manifest.version = std::strtoul(value.c_str(), nullptr, 10);
  if (key == "overlay") manifest.overlay = value == "true" || value == "1";
  if (key == "logs") manifest.logs = value == "true" || value == "1";
  if (key == "listen") manifest.listen = value == "true" || value == "1";
  if (key == "threads") manifest.threads = value == "true" || value == "1";
  if (key == "latch") ::pace(value, manifest);
  if (key == "metrics") manifest.metrics = value != "false" && value != "0";
  if (key == "stdin") manifest.fed = value == "true" || value == "1";
  if (key == "web") manifest.web.push_back(value);
  if (key == "server") manifest.server.push_back(value);
  if (key == "console")
    manifest.console =
      ordinal(value, INPUT::BINDINGS::keys["CONSOLES::DISPATCH::interpret"]);
  if (key == "fullscreen")
    manifest.fullscreen =
      ordinal(value, INPUT::BINDINGS::keys["WINDOW::fullscreen"]);
  if (key == "decorated") manifest.decorated = value != "false" && value != "0";
  if (key == "splash") manifest.splash = value == "true" || value == "1";
  if (key == "size") ::extent(value, manifest);
  if (key == "font")
    manifest.font =
      value == "full" || value == "wide" ? CARTRIDGE::FULL : CARTRIDGE::SLIM;
  if (key == "requires") return CARTRIDGE::MANIFEST::require(value, manifest);
  return 0;
}

auto CARTRIDGE::MANIFEST::parse(
  IO::STREAMS::Input &file, CARTRIDGE::Manifest &manifest,
  Flag folded) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "manifest");
  String line;
  Status status = 0;
  enum Block {
    FLAT,
    WINDOWS,
    COMPONENTS,
    ENGINE,
    PLATFORMS,
    SKIPPED
  } block = FLAT;
  String custom;
  Fold state;
  while (std::getline(file, line)) {
    line = line.substr(0, line.find('#'));
    if (line.starts_with(' ') || line.starts_with('\t')) {
      if (STRING::trim(line).empty()) continue;
      if (block == WINDOWS)
        status |= CARTRIDGE::MANIFEST::window(line, manifest);
      else if (block == COMPONENTS)
        status |= CARTRIDGE::MANIFEST::designate(line, manifest);
      else if (block == ENGINE)
        status |= CARTRIDGE::MANIFEST::demand(line, manifest);
      else if (block == PLATFORMS)
        status |= CARTRIDGE::MANIFEST::fold(line, manifest, state);
      else if (block == FLAT && !custom.empty()) {
        logger.error("Nested section under key \"" + custom + "\".");
        return 1;
      }
      continue;
    }
    block = FLAT;
    custom.clear();
    auto colon = line.find(':');
    if (colon == String::npos) continue;
    const String key = STRING::trim(line.substr(0, colon));
    if (key == "windows") {
      block = WINDOWS;
      continue;
    }
    if (key == "gui") {
      block = COMPONENTS;
      continue;
    }
    if (key == SECTION) {
      block = ENGINE;
      continue;
    }
    if (key == FOLDS) {
      block = folded ? PLATFORMS : SKIPPED;
      continue;
    }
    if (reserved(key)) {
      status |= assign(manifest, key, STRING::trim(line.substr(colon + 1)));
      continue;
    }
    if (manifest.keys.count(key)) {
      logger.error("Repeated key \"" + key + "\".");
      return 1;
    }
    manifest.keys[key] = STRING::trim(line.substr(colon + 1));
    custom = key;
  }
  return status;
}

auto CARTRIDGE::MANIFEST::load(const String &path, Manifest &manifest)
  -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "manifest");
  IO::STREAMS::Input file(path);
  if (!file) {
    logger.error("Cannot read " + path + ".");
    return 1;
  }
  if (parse(file, manifest, true)) return 1;
  logger.debug("Manifest: " + manifest.name + " (" + path + ")");
  if (manifest.version == CARTRIDGE::VERSION) return 0;
  logger.error(
    "Bundle version " + std::to_string(manifest.version) +
    " does not match engine version " + std::to_string(VERSION) + ".");
  return 1;
}
