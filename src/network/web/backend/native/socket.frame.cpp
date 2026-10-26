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
#include <iterator>
#include <network/web/native.socket.internal.hpp>

namespace NETWORK::WEB::NATIVE::SOCKET {
namespace {
constexpr Octet FIN = 0x80;
constexpr Octet MASKED = 0x80;
constexpr Octet OPCODES = 0x0F;
constexpr Octet LENGTH = 0x7F;
constexpr Whole LOW = 0xFF;
constexpr Whole BITS = 8;
constexpr Whole HEADER = 2;
constexpr Whole KEY = 4;
constexpr Integer TAKEN = 1;
constexpr Integer OWED = 0;
constexpr Integer REFUSED = -1;

auto shaped(Whole size) -> const Form & {
  for (const Form &form : FORMS)
    if (size <= form.most) return form;
  return FORMS[std::size(FORMS) - 1];
}

auto width(Octet mark) -> Whole {
  for (const Form &form : FORMS)
    if (form.width && form.mark == mark) return form.width;
  return 0;
}

void append(String &bytes, Whole value, Whole width) {
  for (Whole at = width; at-- > 0;)
    bytes.push_back(static_cast<Char>((value >> (at * BITS)) & LOW));
}

auto read(const String &bytes, Whole from, Whole width) -> Whole {
  Whole value = 0;
  for (Whole at = 0; at < width; ++at)
    value = (value << BITS) | static_cast<Octet>(bytes[from + at]);
  return value;
}
}  // namespace

auto encode(Octet opcode, const String &payload) -> String {
  unsigned char key[KEY];
  if (!random(key, KEY)) return {};
  const Whole size = payload.size();
  const Form &form = shaped(size);
  String bytes(1, static_cast<Char>(FIN | opcode));
  const Whole mark = form.width ? form.mark : size;
  bytes.push_back(static_cast<Char>(MASKED | mark));
  append(bytes, size, form.width);
  bytes.append(reinterpret_cast<const Char *>(key), KEY);
  for (Whole at = 0; at < size; ++at)
    bytes.push_back(static_cast<Char>(payload[at] ^ key[at % KEY]));
  return bytes;
}

auto decode(String &bytes, Frame &frame) -> Integer {
  if (bytes.size() < HEADER) return OWED;
  const auto first = static_cast<Octet>(bytes[0]);
  const auto second = static_cast<Octet>(bytes[1]);
  if (second & MASKED) return REFUSED;
  const Whole wide = width(second & LENGTH);
  const Whole start = HEADER + wide;
  if (bytes.size() < start) return OWED;
  const Whole size = wide ? read(bytes, HEADER, wide) : (second & LENGTH);
  if (size > WEB::SOCKET::CEILING) return REFUSED;
  if (bytes.size() < start + size) return OWED;
  frame.fin = (first & FIN) != 0;
  frame.opcode = first & OPCODES;
  frame.payload = bytes.substr(start, size);
  bytes.erase(0, start + size);
  return TAKEN;
}
}  // namespace NETWORK::WEB::NATIVE::SOCKET
