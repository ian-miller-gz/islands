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
#include <network/web/native.ftp.internal.hpp>

namespace NETWORK::WEB::NATIVE::FTP {
namespace {
constexpr Whole EXTENDED = 229;
constexpr Whole PASSIVE = 227;
constexpr Whole OCTET = 256;
constexpr STRING::Hot FIGURES = "0123456789";

auto parenthetical(const String &text) -> String {
  auto from = text.find('(');
  if (from == String::npos) return {};
  auto to = text.find(')', from);
  if (to == String::npos) return {};
  return text.substr(from + 1, to - from - 1);
}

auto extended(const String &text) -> Whole {
  String inside = parenthetical(text);
  auto first = inside.find_first_of(FIGURES);
  if (first == String::npos) return 0;
  return number(
    inside.substr(first, inside.find_first_not_of(FIGURES, first) - first));
}

auto stated(const String &text) -> Whole {
  String inside = parenthetical(text);
  auto low = inside.rfind(',');
  if (low == String::npos || low == 0) return 0;
  auto high = inside.rfind(',', low - 1);
  if (high == String::npos) return 0;
  return number(inside.substr(high + 1, low - high - 1)) * OCTET +
         number(inside.substr(low + 1));
}
}  // namespace

auto attach(Connection &control, const Url &target, Connection &data) -> Flag {
  Reply modern = speak(control, "EPSV");
  Whole port = modern.code == EXTENDED ? extended(modern.text) : 0;
  if (!port) {
    Reply older = speak(control, "PASV");
    if (older.code == PASSIVE) port = stated(older.text);
  }
  return port && open(target.host, port, data);
}
}  // namespace NETWORK::WEB::NATIVE::FTP
