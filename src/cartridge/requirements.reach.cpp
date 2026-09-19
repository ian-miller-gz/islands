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
#include <chrono>
#include <cartridge/relations.hpp>
#include <cartridge/requirements.internal.hpp>
#include <logger.hpp>
#include <thread>
#define LOGGER_CATEGORY "~/cartridge::requirements"

static constexpr Whole DEADLINE = 5000;
static constexpr Whole INTERVAL = 50;

static auto await(const NETWORK::Wire &where) -> NETWORK::Handle {
  for (Whole waited = 0; waited < DEADLINE; waited += INTERVAL) {
    auto session = NETWORK::connect(where).handle;
    if (session != NETWORK::NONE) return session;
    std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
  }
  return NETWORK::NONE;
}

static auto begin(const CARTRIDGE::Requirement &requirement) -> Status {
  if (requirement.kill)
    return RELATIONS::spawn(REQUIREMENTS::HOST, requirement.bundle) == 0;
  return RELATIONS::start(REQUIREMENTS::HOST, requirement.bundle);
}

auto REQUIREMENTS::reach(const CARTRIDGE::Requirement &requirement)
  -> NETWORK::Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  auto cut = requirement.bundle.rfind('/');
  const String service = cut == String::npos
                           ? requirement.bundle
                           : requirement.bundle.substr(cut + 1);
  const NETWORK::Reach reached = NETWORK::connect(service);
  auto session = reached.handle;
  if (session != NETWORK::NONE) {
    if (!requirement.replace) {
      logger.info("Requirement %s joined.", requirement.bundle.c_str());
      return session;
    }
    if (supplant(session, NETWORK::Wire{reached.wire})) return NETWORK::NONE;
  }
  if (!requirement.ensure || begin(requirement)) return NETWORK::NONE;
  session = await(NETWORK::Wire{reached.wire});
  if (session != NETWORK::NONE)
    logger.info("Requirement %s started.", requirement.bundle.c_str());
  return session;
}
