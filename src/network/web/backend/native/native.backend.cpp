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
#include <network/web/backend.hpp>
#include <network/web/native.ftp.internal.hpp>
#include <network/web/native.http.internal.hpp>

auto NETWORK::WEB::NATIVE::failed(const String &reason, const String &url)
  -> Result {
  return {
    false, 0, {}, "web transfer failed: " + reason + "; requested " + url};
}

namespace {
namespace NATIVE = NETWORK::WEB::NATIVE;
using NETWORK::WEB::Request;
using NETWORK::WEB::Result;
using NETWORK::WEB::Url;

struct Protocol {
  STRING::Hot scheme;
  auto (*exchange)(const Request &, const Url &) -> Result;
};

constexpr Protocol PROTOCOLS[]{
  {"http", NATIVE::HTTP::exchange},
  {"https", NATIVE::HTTP::exchange},
  {"ftp", NATIVE::FTP::exchange},
};
}  // namespace

auto NETWORK::WEB::BACKEND::transfer(const Request &request) -> Result {
  Url target = dissect(request.url);
  for (const Protocol &known : PROTOCOLS)
    if (target.scheme == known.scheme) return known.exchange(request, target);
  return NATIVE::failed(
    target.scheme + " is not a scheme this backend reaches", request.url);
}
