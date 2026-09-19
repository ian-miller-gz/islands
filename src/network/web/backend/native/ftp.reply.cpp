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
constexpr STRING::Hot BREAK = "\r\n";
constexpr Whole DIGITS = 3;

auto complete(const String &text, const String &code, Whole &end) -> Flag {
  String shut = code + " ";
  for (Whole mark = 0; mark + shut.size() <= text.size();) {
    auto stop = text.find(BREAK, mark);
    if (stop == String::npos) return false;
    if (text.compare(mark, shut.size(), shut) == 0) {
      end = stop + 2;
      return true;
    }
    mark = stop + 2;
  }
  return false;
}
}  // namespace

auto hear(Connection &control) -> Reply {
  while (control.pending.size() < CEILING) {
    if (control.pending.size() > DIGITS) {
      String code = control.pending.substr(0, DIGITS);
      if (!number(code)) return {};
      Whole end = 0;
      if (complete(control.pending, code, end)) {
        Reply answer{number(code), control.pending.substr(0, end)};
        control.pending.erase(0, end);
        return answer;
      }
    }
    if (read(control, control.pending) <= 0) return {};
  }
  return {};
}

auto speak(Connection &control, const String &command) -> Reply {
  if (!send(control, command + BREAK)) return {};
  return hear(control);
}
}  // namespace NETWORK::WEB::NATIVE::FTP
