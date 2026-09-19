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
constexpr Whole GREETING = 220;
constexpr Whole WANTS = 331;
constexpr STRING::Hot GUEST = "anonymous";
constexpr STRING::Hot MAILBOX = "islands@";

auto accepted(Whole code) -> Flag { return code >= 200 && code < 300; }
}  // namespace

auto enter(Connection &control) -> Flag {
  if (hear(control).code != GREETING) return false;
  Reply named = speak(control, String("USER ") + GUEST);
  if (named.code == WANTS) named = speak(control, String("PASS ") + MAILBOX);
  return accepted(named.code) && accepted(speak(control, "TYPE I").code);
}
}  // namespace NETWORK::WEB::NATIVE::FTP
