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
#include <network/web/native.http.internal.hpp>

namespace NETWORK::WEB::NATIVE::HTTP {
namespace {
constexpr STRING::Hot BREAK = "\r\n";

auto sized(const String &line, Whole &size) -> Flag {
  String digits = line.substr(0, line.find(';'));
  const auto *edge = digits.data() + digits.size();
  auto read = std::from_chars(digits.data(), edge, size, 16);
  return read.ec == std::errc() && read.ptr == edge;
}
}  // namespace

auto unchunk(const String &body, String &whole) -> Flag {
  for (Whole mark = 0; mark < body.size();) {
    auto end = body.find(BREAK, mark);
    if (end == String::npos) return false;
    Whole size = 0;
    if (!sized(body.substr(mark, end - mark), size)) return false;
    if (!size) return true;
    mark = end + 2;
    if (mark + size > body.size()) return false;
    whole.append(body, mark, size);
    mark += size + 2;
  }
  return false;
}
}  // namespace NETWORK::WEB::NATIVE::HTTP
