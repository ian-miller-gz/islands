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
#pragma once
#include <common.hpp>
#include <network/sockets/descriptor.internal.hpp>
#include <network/web/loopback.internal.hpp>

namespace NETWORK::WEB::LOOPBACK {
using Moment = long long;
constexpr Whole LINKS = 4;
constexpr Whole HEAD = 8u << 10;
constexpr Moment IDLE = 10000;
constexpr STRING::Hot BREAK = "\r\n\r\n";

struct Link {
  Descriptor descriptor = CLOSED;
  String pending;
  String outbox;
  Flag answered = false;
  Moment active = 0;
};

struct Lane {
  Descriptor listener = CLOSED;
  String path;
  Answering answering = nullptr;
  Flag asked = false;
  Vector<Link> links;
};

struct Reply {
  String bytes;
  std::optional<String> query;
};

auto lane() -> Lane &;
auto now() -> Moment;
auto read(Link &link) -> Flag;
auto flushing(Link &link) -> Flag;
auto answered(const String &head, const Lane &one) -> Reply;
}  // namespace NETWORK::WEB::LOOPBACK
