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

#include <deque>

#include <metrics.hpp>

namespace METRICS {

struct Series {
  std::deque<Sample> window;
  Sample pending = 0;
  Flag counted = false;

  void push(Sample value) {
    window.push_back(value);
    if (window.size() > HISTORY) window.pop_front();
  }
};

auto registry() -> Map<String, Series> &;

extern Flag on;

namespace HARDWARE {
void begin(STRING::Hot stage);
void end(STRING::Hot stage);
void reset();
}  // namespace HARDWARE

}  // namespace METRICS
