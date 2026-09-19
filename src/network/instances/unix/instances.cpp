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
#include <network/instances/unix/instances.internal.hpp>
#include <roster/roster.hpp>

namespace INSTANCES = NETWORK::INSTANCES;

static void drop(INSTANCES::STATE::Link &link) {
  if (link.session == NETWORK::NONE) return;
  NETWORK::SESSIONS::destroy(link.session);
  link.session = NETWORK::NONE;
  link.buffer.clear();
}

auto INSTANCES::refresh() -> Flag {
  auto &link = STATE::daemon;
  if (link.session != NETWORK::NONE) return true;
  link.session = NETWORK::connect(ROSTER::NAME).handle;
  if (link.session == NETWORK::NONE) return false;
  if (NETWORK::SESSIONS::send(link.session, String(ROSTER::LIST) + "\n"))
    return true;
  drop(link);
  return false;
}

auto INSTANCES::attach(const String &pid) -> Flag {
  detach();
  for (const auto &entry : STATE::entries) {
    if (entry.pid != pid) continue;
    STATE::instance.session =
      NETWORK::connect(NETWORK::Wire{entry.socket}).handle;
    if (STATE::instance.session == NETWORK::NONE) return false;
    STATE::peer = pid;
    return true;
  }
  return false;
}

void INSTANCES::detach() {
  drop(STATE::instance);
  STATE::peer.clear();
}

auto INSTANCES::send(const String &line) -> Flag {
  if (STATE::instance.session == NETWORK::NONE) return false;
  if (NETWORK::SESSIONS::send(STATE::instance.session, line + "\n"))
    return true;
  detach();
  ++STATE::drops;
  return false;
}

void INSTANCES::close() {
  drop(STATE::daemon);
  detach();
  STATE::entries.clear();
  STATE::replies.clear();
  STATE::listings = 0;
  STATE::drops = 0;
  STATE::shown = 0;
  STATE::mourned = 0;
}
