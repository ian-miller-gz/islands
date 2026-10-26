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
#include <network/web/loopback.link.internal.hpp>

namespace NETWORK::WEB::LOOPBACK {
namespace {
using Heard = std::optional<String>;

void accept(Lane &one) {
  while (one.listener != CLOSED && one.links.size() < LINKS) {
    const Descriptor descriptor = NETWORK::accepted(one.listener);
    if (descriptor == CLOSED) return;
    one.links.push_back({.descriptor = descriptor, .active = now()});
  }
}

auto moved(Link &link, Lane &one, Heard &heard) -> Flag {
  if (link.answered) return flushing(link);
  if (one.asked || !read(link)) return false;
  const auto ending = link.pending.find(BREAK);
  if (ending == String::npos) return link.pending.size() <= HEAD;
  Reply reply = answered(link.pending.substr(0, ending), one);
  one.asked = one.asked || reply.query.has_value();
  if (reply.query) heard = reply.query;
  link.outbox = reply.bytes;
  link.answered = true;
  return !link.outbox.empty() && flushing(link);
}

auto stepped(Link &link, Lane &one, Heard &heard, Moment moment) -> Flag {
  if (moment - link.active > IDLE) return false;
  const Whole moving = link.pending.size() + link.outbox.size();
  const Flag alive = moved(link, one, heard);
  if (link.pending.size() + link.outbox.size() != moving) link.active = moment;
  return alive;
}
}  // namespace

auto flushing(Link &link) -> Flag {
  const auto wrote =
    ::send(link.descriptor, link.outbox.data(), link.outbox.size(), QUIET);
  if (wrote > 0) link.outbox.erase(0, static_cast<Whole>(wrote));
  return !link.outbox.empty() && (wrote > 0 || failing() == AGAIN);
}

auto advance() -> std::optional<String> {
  Lane &one = lane();
  accept(one);
  Heard heard;
  const Moment moment = now();
  std::erase_if(one.links, [&](Link &link) {
    if (stepped(link, one, heard, moment)) return false;
    NETWORK::close(link.descriptor);
    return true;
  });
  if (one.asked && one.listener != CLOSED) {
    NETWORK::close(one.listener);
    one.listener = CLOSED;
  }
  return heard;
}
}  // namespace NETWORK::WEB::LOOPBACK
