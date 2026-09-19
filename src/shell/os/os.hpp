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
#pragma once

#include <shell/os/process.hpp>
#include <shell/os/pty.hpp>
#include <shell/os/keys.hpp>
#include <shell/os/screen.hpp>
#include <common.hpp>

namespace CONSOLES {
struct Console;
}

namespace SHELL::OS {

constexpr STRING::Hot PROMPT = "$ ";

auto quote(const String &text) -> String;

#if SR_PLATFORM != SR_WEB
auto run(const String &line) -> Status;
#else
inline auto run(const String &) -> Status { return 1; }
#endif

void interpret(CONSOLES::Console &console, const String &line);

void adopt(CONSOLES::Console &console);

void pump(CONSOLES::Console &console);

void close();

}  // namespace SHELL::OS
