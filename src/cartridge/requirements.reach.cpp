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
#include <cartridge/relations.hpp>
#include <cartridge/requirements.internal.hpp>
#include <chrono>
#include <filesystem>
#include <logger.hpp>
#include <thread>
#define LOGGER_CATEGORY "~/cartridge::requirements"

using Clock = std::chrono::steady_clock;

static constexpr std::chrono::milliseconds DEADLINE{5000};
static constexpr std::chrono::milliseconds INTERVAL{50};

// The deadline is wall time: a dial that blocks (a loopback refusal Winsock's
// poll never reports costs its whole patience) still ends the wait on time.
static auto await(const NETWORK::Wire &where) -> NETWORK::Handle {
  const auto limit = Clock::now() + DEADLINE;
  for (;;) {
    auto session = NETWORK::connect(where).handle;
    if (session != NETWORK::NONE) return session;
    if (Clock::now() >= limit) return NETWORK::NONE;
    std::this_thread::sleep_for(INTERVAL);
  }
}

// A bundle the host cannot load here (no entry file for this platform) is
// unavailable at once: starting a host for it would only burn the deadline.
// The bundle folded into the reef host (SR_REEF_BUNDLE, the launcher's
// monitor on a static delivery) needs no entry file: the host carries it.
static auto present(const String &bundle) -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
#ifdef SR_REEF_BUNDLE
  if (bundle == SR_REEF_BUNDLE) return true;
#endif
  const auto manifest = CARTRIDGE::MANIFEST::read(bundle);
  const String entry = CARTRIDGE::MANIFEST::entry(bundle, manifest);
  std::error_code ec;
  if (!entry.empty() && std::filesystem::exists(entry, ec)) return true;
  logger.error(
    "Requirement %s has no entry for this platform.", bundle.c_str());
  return false;
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
  if (!requirement.ensure || !present(requirement.bundle) || begin(requirement))
    return NETWORK::NONE;
  session = await(NETWORK::Wire{reached.wire});
  if (session != NETWORK::NONE)
    logger.info("Requirement %s started.", requirement.bundle.c_str());
  return session;
}
