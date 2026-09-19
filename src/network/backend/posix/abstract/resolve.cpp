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
#include <network/listeners.hpp>
#include <network/protocols.hpp>
#include <network/reach.hpp>
#include <network/sessions.hpp>

static constexpr STRING::Hot SCOPE = "islands/";

auto NETWORK::ABSTRACT::resolve(const String &service) -> Anchor {
  return {SCOPE + service};
}

auto NETWORK::ABSTRACT::parse(const String &wire) -> Anchor { return {wire}; }

auto NETWORK::ABSTRACT::format(const Anchor &anchor) -> String {
  return anchor.name;
}

namespace {
auto dialed(const String &wire) -> NETWORK::Handle {
  return NETWORK::SESSIONS::create(NETWORK::ABSTRACT::parse(wire));
}

auto listened(const String &wire, Whole backlog) -> NETWORK::Handle {
  return NETWORK::LISTENERS::create(NETWORK::ABSTRACT::parse(wire), backlog);
}

auto resolved(const String &service) -> String {
  return NETWORK::ABSTRACT::format(NETWORK::ABSTRACT::resolve(service));
}

[[maybe_unused]] const Flag ENROLLED = NETWORK::enroll(
  {NETWORK::Transport::ABSTRACT, &dialed, &listened, &resolved});
}  // namespace
