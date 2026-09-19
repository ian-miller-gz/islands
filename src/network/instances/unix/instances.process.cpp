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

static auto split(const String &line) -> Vector<String> {
  Vector<String> found;
  Whole begin = 0;
  for (auto end = line.find(ROSTER::TAB); end != String::npos;
       end = line.find(ROSTER::TAB, begin)) {
    found.push_back(line.substr(begin, end - begin));
    begin = end + 1;
  }
  found.push_back(line.substr(begin));
  return found;
}

static void record(const String &line) {
  auto parts = split(line);
  if (parts.size() != ROSTER::FIELDS) return;
  INSTANCES::STATE::entries.push_back({parts[0], parts[1], parts[2], parts[3]});
}

static void parse() {
  INSTANCES::STATE::entries.clear();
  auto &buffer = INSTANCES::STATE::daemon.buffer;
  if (!buffer.empty() && buffer.back() != '\n') buffer += '\n';
  for (auto cut = buffer.find('\n'); cut != String::npos;
       cut = buffer.find('\n')) {
    String line = buffer.substr(0, cut);
    buffer.erase(0, cut + 1);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (!line.empty()) record(line);
  }
  ++INSTANCES::STATE::listings;
}

static void drain() {
  auto &link = INSTANCES::STATE::daemon;
  if (link.session == NETWORK::NONE) return;
  if (NETWORK::SESSIONS::receive(link.session, link.buffer)) return;
  NETWORK::SESSIONS::destroy(link.session);
  link.session = NETWORK::NONE;
  parse();
  link.buffer.clear();
}

static void gather() {
  auto &link = INSTANCES::STATE::instance;
  if (link.session == NETWORK::NONE) return;
  Flag alive = NETWORK::SESSIONS::receive(link.session, link.buffer);
  if (!alive && !link.buffer.empty() && link.buffer.back() != '\n')
    link.buffer += '\n';
  for (auto cut = link.buffer.find('\n'); cut != String::npos;
       cut = link.buffer.find('\n')) {
    String line = link.buffer.substr(0, cut);
    link.buffer.erase(0, cut + 1);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    INSTANCES::STATE::replies.push_back(line);
  }
  if (alive) return;
  INSTANCES::detach();
  ++INSTANCES::STATE::drops;
}

void INSTANCES::process() {
  drain();
  gather();
}
