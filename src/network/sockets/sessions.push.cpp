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
#include <network/sockets/sessions.internal.hpp>

auto NETWORK::SESSIONS::push(Handle session, const String &data) -> Flag {
  Descriptor descriptor = fetch(session);
  if (descriptor == CLOSED) return false;
  if (const Codec *codec = codecs[session])
    return codec->push(descriptor, data);
  for (Whole sent = 0; sent < data.size();) {
    auto wrote =
      ::send(descriptor, data.data() + sent, data.size() - sent, HURRIED);
    if (wrote > 0) {
      sent += wrote;
      continue;
    }
    if (failing() == INTERRUPTED) continue;
    return false;
  }
  return true;
}
