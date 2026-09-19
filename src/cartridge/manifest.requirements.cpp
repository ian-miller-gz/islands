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
#include <cartridge/manifest.hpp>
#include <cartridge/manifest.internal.hpp>
#include <logger.hpp>
#include <sstream>
#include <string>
#define LOGGER_CATEGORY "~/cartridge::"

static auto acquire(CARTRIDGE::Requirement &requirement, const String &value)
  -> Flag {
  if (value != "adopt" && value != "start" && value != "replace") return false;
  requirement.ensure = value != "adopt";
  requirement.replace = value == "replace";
  return true;
}

static auto close(CARTRIDGE::Requirement &requirement, const String &value)
  -> Flag {
  if (value != "kill" && value != "release") return false;
  requirement.kill = value == "kill";
  return true;
}

static auto flag(CARTRIDGE::Requirement &requirement, const String &token)
  -> Flag {
  if (token == "ensure") return requirement.ensure = true;
  if (token == "vital") return requirement.vital = true;
  return false;
}

static auto policy(
  std::istringstream &words, CARTRIDGE::Requirement &requirement,
  const String &token) -> Flag {
  String value;
  if (token == "acquire:") return words >> value && acquire(requirement, value);
  if (token == "close:") return words >> value && close(requirement, value);
  return flag(requirement, token);
}

auto CARTRIDGE::MANIFEST::require(const String &value, Manifest &manifest)
  -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "manifest");
  std::istringstream words(value);
  CARTRIDGE::Requirement requirement;
  words >> requirement.bundle;
  if (requirement.bundle.empty()) {
    logger.error("A requires entry names no bundle.");
    return 1;
  }
  String token;
  while (words >> token)
    if (!policy(words, requirement, token)) {
      logger.error("Unknown requirement policy \"" + token + "\".");
      return 1;
    }
  manifest.requirements.push_back(requirement);
  return 0;
}
