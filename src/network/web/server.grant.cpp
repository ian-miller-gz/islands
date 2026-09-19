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
#include <network/web/server.internal.hpp>
#include <algorithm>
#include <cstring>

namespace {
Map<String, String> grants;

constexpr Whole SEXTET = 6;
constexpr Whole OCTET = 8;
constexpr STRING::Hot ALPHABET =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const String SCHEME = "Basic ";

auto decoded(const String &text) -> String {
  String plain;
  Whole bits = 0;
  Whole held = 0;
  for (Char letter : text) {
    if (letter == '=') break;
    const Char *seat = letter ? std::strchr(ALPHABET, letter) : nullptr;
    if (!seat) return {};
    bits = (bits << SEXTET) | static_cast<Whole>(seat - ALPHABET);
    held += SEXTET;
    if (held < OCTET) continue;
    held -= OCTET;
    plain.push_back(static_cast<Char>((bits >> held) & 0xFFu));
  }
  return plain;
}

auto same(const String &left, const String &right) -> Flag {
  Whole difference =
    static_cast<Whole>(left.size()) ^ static_cast<Whole>(right.size());
  const auto span = std::min(left.size(), right.size());
  for (decltype(left.size()) index = 0; index < span; ++index)
    difference |= static_cast<Whole>(
      static_cast<unsigned char>(left[index]) ^
      static_cast<unsigned char>(right[index]));
  return difference == 0;
}
}  // namespace

void NETWORK::WEB::SERVER::grant(const String &reader, const String &secret) {
  if (!reader.empty()) grants[reader] = secret;
}

void NETWORK::WEB::SERVER::revoke(const String &reader) {
  grants.erase(reader);
}

void NETWORK::WEB::SERVER::disarm() { grants.clear(); }

auto NETWORK::WEB::SERVER::resolve(const String &credential) -> String {
  if (!credential.starts_with(SCHEME)) return {};
  const String plain = decoded(STRING::trim(credential.substr(SCHEME.size())));
  const auto colon = plain.find(':');
  if (colon == String::npos) return {};
  const String name = plain.substr(0, colon);
  const String offered = plain.substr(colon + 1);
  const auto armed = grants.find(name);
  const Flag matched =
    same(armed == grants.end() ? offered : armed->second, offered);
  return armed != grants.end() && matched ? name : String();
}

auto NETWORK::WEB::SERVER::GET::readers() -> Vector<String> {
  Vector<String> named;
  named.reserve(grants.size());
  for (const auto &[reader, secret] : grants) named.push_back(reader);
  std::sort(named.begin(), named.end());
  return named;
}
