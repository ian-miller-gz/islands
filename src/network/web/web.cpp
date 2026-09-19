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
#include <network/web.hpp>
#include <network/web/backend.hpp>
#include <network/web/backend/selection.hpp>
#include <network/web/web.internal.hpp>

namespace NETWORK::WEB {

namespace {
constexpr Whole HOPS = 5;

auto refuse(const String &url) -> Result {
  return {
    false,
    0,
    {},
    "web transfer refused: no backend compiled "
    "(SR_WEB_BACKEND: SR_NONE); requested " +
      url};
}

auto realize(const Request &request) -> Result {
#if SR_WEB_BACKEND == SR_NONE
  return refuse(request.url);
#else
  return BACKEND::transfer(request);
#endif
}
}  // namespace

auto get(const String &url) -> Result { return transfer({url, "GET", {}, {}}); }

auto put(const String &url, const String &body) -> Result {
  return transfer({url, "PUT", {}, body});
}

auto post(const String &url, const String &body) -> Result {
  return transfer({url, "POST", {}, body});
}

auto transfer(const Request &request) -> Result {
  Request hop = request;
  for (Whole step = 0; step <= HOPS; ++step) {
    if (auto denied = screen(hop.url)) return {false, 0, {}, *denied};
    Result result = realize(hop);
    if (result.redirect.empty()) return result;
    hop.url = result.redirect;
  }
  return {
    false,
    0,
    {},
    "web transfer refused: more than " + std::to_string(HOPS) +
      " redirects; requested " + request.url};
}

}  // namespace NETWORK::WEB
