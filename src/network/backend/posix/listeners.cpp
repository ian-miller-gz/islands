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
#include <network/backend/posix/listeners.internal.hpp>
#include <network/backend/posix/sessions.internal.hpp>

using NETWORK::LISTENERS::entries;

static auto slot() -> NETWORK::Handle {
  for (Whole index = 0; index < entries.size(); ++index)
    if (entries[index].descriptor == NETWORK::CLOSED) return index;
  entries.emplace_back();
  return entries.size() - 1;
}

auto NETWORK::LISTENERS::adopt(
  Descriptor descriptor, const String &token, Releaser releaser,
  Acceptor acceptor) -> Handle {
  Handle taken = slot();
  entries[taken] = {descriptor, token, releaser, acceptor};
  return taken;
}

auto NETWORK::LISTENERS::accept(Handle listener) -> Handle {
  if (listener >= entries.size() || entries[listener].descriptor == CLOSED)
    return NONE;
  Descriptor taken = accepted(entries[listener].descriptor);
  if (taken == CLOSED) return NONE;
  Acceptor acceptor = entries[listener].acceptor;
  return acceptor ? acceptor(taken) : SESSIONS::adopt(taken);
}

void NETWORK::LISTENERS::destroy(Handle listener) {
  if (listener >= entries.size() || entries[listener].descriptor == CLOSED)
    return;
  close(entries[listener].descriptor);
  if (entries[listener].releaser)
    entries[listener].releaser(entries[listener].token);
  entries[listener] = {};
}
