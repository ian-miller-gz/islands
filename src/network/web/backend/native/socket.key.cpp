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
#include <mbedtls/base64.h>
#include <mbedtls/sha1.h>

#include <network/web/native.socket.internal.hpp>

namespace NETWORK::WEB::NATIVE::SOCKET {
namespace {
constexpr Whole DIGEST = 20;
constexpr Whole ENCODED = 64;

auto encoded(const unsigned char *bytes, Whole size) -> String {
  unsigned char text[ENCODED];
  size_t length = 0;
  if (mbedtls_base64_encode(text, ENCODED, &length, bytes, size) != 0)
    return {};
  return String(reinterpret_cast<const char *>(text), length);
}
}  // namespace

auto nonce() -> String {
  unsigned char bytes[NONCE];
  return random(bytes, NONCE) ? encoded(bytes, NONCE) : String{};
}

auto expected(const String &key) -> String {
  const String joined = key + GUID;
  unsigned char digest[DIGEST];
  const auto *text = reinterpret_cast<const unsigned char *>(joined.data());
  if (mbedtls_sha1(text, joined.size(), digest) != 0) return {};
  return encoded(digest, DIGEST);
}
}  // namespace NETWORK::WEB::NATIVE::SOCKET
