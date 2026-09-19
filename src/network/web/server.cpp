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
#include <logger.hpp>
#include <network/web/selection.hpp>
#include <network/web/server.internal.hpp>
#define LOGGER_CATEGORY "~/network::serve"

namespace NETWORK::WEB::SERVER {
namespace {
Flag standing = false;
}  // namespace

auto open(Handler answering) -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (standing) return true;
  const Declared &lane = declared();
  if (!answering || lane.address.empty()) {
    logger.error(
      "Serving refused: " + String(
                              answering
                                ? "no lane declared (the manifest server: key)"
                                : "no answering function given"));
    return false;
  }
#if SR_SERVER == SR_NONE
  logger.error(
    "Serving refused: no serving path compiled (SR_SERVER: SR_NONE); "
    "declared " +
    lane.address + ":" + std::to_string(lane.port));
  return false;
#else
  standing = bind(lane, answering);
  if (standing) logger.info("Serving on " + bound());
  return standing;
#endif
}

void process() {
#if SR_SERVER != SR_NONE
  if (standing) advance();
#endif
}

void close() {
#if SR_SERVER != SR_NONE
  if (standing) release();
#endif
  standing = false;
  disarm();
}

auto GET::serving() -> Flag { return standing; }

auto GET::wire() -> String {
#if SR_SERVER == SR_NONE
  return {};
#else
  return standing ? bound() : String();
#endif
}

}  // namespace NETWORK::WEB::SERVER
