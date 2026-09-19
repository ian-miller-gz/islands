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
#include <cartridge/requirements.hpp>
#include <cartridge/requirements.internal.hpp>
#include <logger.hpp>
#include <stdexcept>
#include <string>
#define LOGGER_CATEGORY "~/cartridge::requirements"

static auto alive(REQUIREMENTS::Watch &watch) -> Flag {
  String noise;
  return NETWORK::SESSIONS::receive(watch.session, noise);
}

static void died(const CARTRIDGE::Requirement &requirement) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.error("Requirement %s died.", requirement.bundle.c_str());
  if (requirement.vital)
    throw std::runtime_error("Requirement " + requirement.bundle + " died.");
}

void REQUIREMENTS::process() {
  for (Whole at = 0; at < watches.size();)
    if (alive(watches[at])) {
      ++at;
    } else {
      NETWORK::SESSIONS::destroy(watches[at].session);
      const auto requirement = watches[at].requirement;
      watches.erase(watches.begin() + at);
      died(requirement);
    }
}
