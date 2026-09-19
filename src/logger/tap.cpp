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
#include <logger/tap.internal.hpp>

namespace {
constexpr Whole BACKLOG = 256;
constexpr Whole HISTORY = 256;
Flag allowed = false;
Flag opened = false;
Vector<String> lines;
Whole lost = 0;
Vector<String> past;
}  // namespace

void LOGGER::TAP::adopt(Flag logs) {
  allowed = logs;
  if (!allowed) close();
}

auto LOGGER::TAP::open() -> Status {
  if (!allowed) return 1;
  opened = true;
  return 0;
}

void LOGGER::TAP::close() {
  opened = false;
  lines.clear();
  lost = 0;
}

auto LOGGER::TAP::receive() -> Vector<String> {
  Vector<String> drained;
  drained.swap(lines);
  return drained;
}

void LOGGER::TAP::push(const String &line) {
  past.push_back(line);
  if (past.size() > HISTORY) past.erase(past.begin());
  if (!opened) return;
  if (lines.size() >= BACKLOG) {
    ++lost;
    return;
  }
  lines.push_back(line);
}

auto LOGGER::TAP::recent(Whole count) -> Vector<String> {
  if (count == 0 || count >= past.size()) return past;
  return {past.end() - count, past.end()};
}

auto LOGGER::TAP::GET::dropped() -> Whole { return lost; }
