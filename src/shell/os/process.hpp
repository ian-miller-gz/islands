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

#include <common.hpp>
#include <common/platform/selection.hpp>

namespace SHELL::OS {

#if SR_PLATFORM == SR_WINDOWS
struct Process {
  void *child = nullptr;
  void *in = nullptr;
  void *out = nullptr;
  String rest;
};
#else
struct Process {
  int child = -1;
  int in = -1;
  int out = -1;
  String rest;
};
#endif

namespace PROCESS {
#if SR_PLATFORM != SR_WEB
void spawn(Process &process);

Flag running(const Process &process);

void feed(Process &process, const String &line);

auto harvest(Process &process) -> Vector<String>;

void stop(Process &process);
#else
inline void spawn(Process &) {}
inline Flag running(const Process &) { return false; }
inline void feed(Process &, const String &) {}
inline auto harvest(Process &) -> Vector<String> { return {}; }
inline void stop(Process &) {}
#endif
}  // namespace PROCESS

}  // namespace SHELL::OS
