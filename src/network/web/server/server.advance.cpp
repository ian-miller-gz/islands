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
#include <network/web/server.link.internal.hpp>

namespace NETWORK::WEB::SERVER {
namespace {
auto seated(Lane &one, Link *link) -> Flag {
  for (Link *&seat : one.links)
    if (!seat) {
      seat = link;
      return true;
    }
  if (one.links.size() >= LINKS) return false;
  one.links.push_back(link);
  return true;
}

auto vacant(const Lane &one) -> Flag {
  for (const Link *seat : one.links)
    if (!seat) return true;
  return one.links.size() < LINKS;
}

void accepted(Lane &one) {
  for (Whole taken = 0; taken < LINKS && vacant(one); ++taken) {
    Descriptor reader = NETWORK::accepted(one.listener);
    if (reader == CLOSED) return;
    const Moment moment = now();
    seated(
      one, new Link{.descriptor = reader, .opened = moment, .active = moment});
  }
}

auto moved(Link &link, Handler answering) -> Flag {
  if (!link.established) return handshake(link);
  if (!link.outbox.empty()) return flushing(link);
  if (!read(link)) return false;
  const auto ending = link.pending.find(BREAK);
  if (ending == String::npos) {
    if (link.pending.size() <= HEAD) return true;
    link.outbox = composed({{413, "text/plain; charset=utf-8", {}}, true});
    return flushing(link);
  }
  link.outbox = composed(answered(link.pending.substr(0, ending), answering));
  return flushing(link);
}

auto stepped(Link &link, Handler answering, Moment moment) -> Flag {
  if (moment - link.active > IDLE) return false;
  const auto heard = link.pending.size();
  const auto owed = link.outbox.size();
  const Flag stood = link.established;
  const Flag alive = moved(link, answering);
  if (
    link.pending.size() != heard || link.outbox.size() != owed ||
    link.established != stood)
    link.active = moment;
  return alive;
}
}  // namespace

void advance() {
  Lane &one = lane();
  accepted(one);
  const Moment moment = now();
  for (Link *&seat : one.links) {
    if (!seat || stepped(*seat, one.answering, moment)) continue;
    shut(*seat);
    delete seat;
    seat = nullptr;
  }
}

}  // namespace NETWORK::WEB::SERVER
