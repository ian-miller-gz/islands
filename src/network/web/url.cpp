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
#include <charconv>
#include <network/web/url.internal.hpp>

auto NETWORK::WEB::number(const String &digits) -> Whole {
  Whole value = 0;
  const auto *edge = digits.data() + digits.size();
  auto read = std::from_chars(digits.data(), edge, value);
  return (read.ec == std::errc() && read.ptr == edge) ? value : 0;
}

namespace {
using NETWORK::WEB::number;

auto authority(const String &text, Whole &port) -> String {
  if (text.starts_with("[")) {
    auto shut = text.find(']');
    if (shut == String::npos) return {};
    if (auto colon = text.find(':', shut); colon != String::npos)
      port = number(text.substr(colon + 1));
    return text.substr(1, shut - 1);
  }
  auto colon = text.find(':');
  if (colon == String::npos) return text;
  port = number(text.substr(colon + 1));
  return text.substr(0, colon);
}
}  // namespace

auto NETWORK::WEB::lookup(const String &name) -> const Scheme * {
  for (const Scheme &known : SCHEMES)
    if (name == known.name) return &known;
  return nullptr;
}

auto NETWORK::WEB::dissect(const String &url) -> Url {
  auto mark = url.find("://");
  if (mark == String::npos) return {};
  Url target{url.substr(0, mark), {}, 0, "/"};
  String rest = url.substr(mark + 3);
  if (auto cut = rest.find_first_of("/?#"); cut != String::npos) {
    target.path = (rest[cut] == '/' ? "" : "/") + rest.substr(cut);
    rest = rest.substr(0, cut);
  }
  target.host = authority(rest, target.port);
  if (!target.port)
    if (const Scheme *known = lookup(target.scheme)) target.port = known->port;
  return target;
}

auto NETWORK::WEB::rebase(const Url &origin, const String &location) -> String {
  if (location.find("://") != String::npos) return location;
  const Scheme *known = lookup(origin.scheme);
  String base = origin.scheme + "://" + origin.host;
  if (!known || origin.port != known->port)
    base += ":" + std::to_string(origin.port);
  if (location.starts_with("/")) return base + location;
  return base + origin.path.substr(0, origin.path.rfind('/') + 1) + location;
}
