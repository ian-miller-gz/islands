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
#include <network/web/server.link.internal.hpp>

namespace NETWORK::WEB::SERVER {

auto lane() -> Lane & {
  static Lane one;
  return one;
}

auto now() -> Moment {
  using std::chrono::steady_clock;
  return std::chrono::duration_cast<std::chrono::milliseconds>(
           steady_clock::now().time_since_epoch())
    .count();
}

auto bind(const Declared &declared, Handler answering) -> Flag {
  Lane &one = lane();
  one.certificate = certificate(declared);
  if (!one.certificate) return false;
  one.listener = listening(declared);
  if (one.listener == CLOSED) return false;
  one.wire = bound(one.listener, declared.address);
  one.answering = answering;
  return true;
}

void release() {
  Lane &one = lane();
  for (Link *link : one.links)
    if (link) {
      shut(*link);
      delete link;
    }
  one.links.clear();
  if (one.listener != CLOSED) NETWORK::close(one.listener);
  one.listener = CLOSED;
  one.answering = nullptr;
  one.wire.clear();
}

auto bound() -> String { return lane().wire; }

}  // namespace NETWORK::WEB::SERVER
