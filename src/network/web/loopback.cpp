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
#include <network/web/loopback.internal.hpp>
#define LOGGER_CATEGORY "~/network::loopback"

namespace NETWORK::WEB::LOOPBACK {
namespace {
Flag declared = false;
Flag standing = false;
String heard;

auto refuse(STRING::Hot reason) -> String {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.error(String("Loopback refused: ") + reason);
  return {};
}
}  // namespace

void adopt(Flag lane) { declared = lane; }

auto open(const String &path, Answering answering) -> String {
  close();
  if (!declared) return refuse("no lane declared");
  if (!answering || !path.starts_with("/")) return refuse("no path answered");
#if SR_SERVER == SR_NONE
  return refuse("no listener compiled");
#else
  const Whole port = bind(path, answering);
  if (port == 0) return {};
  standing = true;
  return SCHEME + String(ADDRESS) + ":" + std::to_string(port) + path;
#endif
}

auto process() -> Flag {
  if (!standing) return false;
#if SR_SERVER != SR_NONE
  if (auto query = advance()) {
    heard = *query;
    return true;
  }
#endif
  return false;
}

void close() {
#if SR_SERVER != SR_NONE
  if (standing) release();
#endif
  standing = false;
  heard.clear();
}

auto GET::query() -> String { return heard; }
}  // namespace NETWORK::WEB::LOOPBACK
