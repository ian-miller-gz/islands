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
#include <cartridge/requirements.hpp>
#include <cartridge/requirements.internal.hpp>
#include <logger.hpp>
#include <stdexcept>
#include <string>
#define LOGGER_CATEGORY "~/cartridge::requirements"

static void require(const CARTRIDGE::Requirement &requirement) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  auto session = REQUIREMENTS::reach(requirement);
  if (session != NETWORK::NONE)
    return REQUIREMENTS::watches.push_back({requirement, session});
  logger.error("Requirement %s is unavailable.", requirement.bundle.c_str());
  if (requirement.vital)
    throw std::runtime_error(
      "Requirement " + requirement.bundle + " is unavailable.");
}

void REQUIREMENTS::initialize() {
  if (cycles()) throw std::runtime_error("Requirement cycle.");
  for (const auto &requirement : CARTRIDGE::requirements) require(requirement);
}

void REQUIREMENTS::close() {
  for (auto &watch : watches) NETWORK::SESSIONS::destroy(watch.session);
  watches.clear();
}

auto REQUIREMENTS::GET::met(const String &bundle) -> Flag {
  for (const auto &watch : watches)
    if (watch.requirement.bundle == bundle) return true;
  return false;
}
