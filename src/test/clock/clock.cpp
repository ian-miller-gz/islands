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
#include <common.hpp>
#include <chrono>
#include <iostream>
#include <thread>

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

void tick() { std::this_thread::sleep_for(std::chrono::milliseconds(2)); }

using CLOCK::GET::delta;
using CLOCK::GET::elapsed;
using CLOCK::GET::frame;
using CLOCK::GET::rate;

}  // namespace

int main() {
  using namespace CLOCK;
  Status status = 0;

  reset();
  status |= check(
    "initial: index, delta, elapsed, rate all zero",
    frame() == 0 && delta() == 0.0f && elapsed() == 0.0f && rate() == 0.0f);

  stamp();
  status |= check(
    "first pass: delta and elapsed zero, index zero",
    delta() == 0.0f && elapsed() == 0.0f && frame() == 0);

  tick();
  stamp();
  status |= check(
    "second pass: index one, delta and elapsed positive",
    frame() == 1 && delta() > 0.0f && elapsed() > 0.0f);

  const Float d = delta(), e = elapsed(), r = rate();
  const Whole f = frame();
  status |= check(
    "one stamp per pass: repeated reads agree",
    delta() == d && elapsed() == e && rate() == r && frame() == f);

  Flag monotonic = true;
  Whole passes = frame();
  Float seen = elapsed();
  for (Whole lap = 0; lap < 4; lap += 1) {
    tick();
    stamp();
    monotonic = monotonic && frame() == passes + 1 && delta() >= 0.0f &&
                elapsed() >= seen;
    passes = frame();
    seen = elapsed();
  }
  status |=
    check("monotonic run: index steps one, elapsed never backward", monotonic);
  status |=
    check("rate: positive after passes with positive delta", rate() > 0.0f);

  Flag ranged = true, still = true, paired = true;
  for (Whole span = 1; span <= 6; span += 1) {
    ranged = ranged && sequence(span, 0.01f) < span;
    still = still && sequence(span, 0.0f) == 0 && sequence(span, -1.0f) == 0;
    paired = paired && pulse(0.01f * span) == (sequence(2, 0.01f * span) == 0);
  }
  status |= check("sequence: index stays in [0, count)", ranged);
  status |= check(
    "sequence: zero count or non-positive period holds frame zero",
    sequence(0, 0.01f) == 0 && still);
  status |= check(
    "pulse: the two-phase sequence's first phase, false when period <= 0",
    paired && !pulse(0.0f) && !pulse(-1.0f));
  status |=
    check("single-frame sequence: always frame zero", sequence(1, 0.01f) == 0);

  reset();
  status |= check(
    "reset: index, delta, elapsed, rate all zero",
    frame() == 0 && delta() == 0.0f && elapsed() == 0.0f && rate() == 0.0f);
  stamp();
  status |= check(
    "reset then stamp: first pass zero again",
    delta() == 0.0f && elapsed() == 0.0f && frame() == 0);

  std::cout << "clock relay: " << (status ? "FAIL" : "PASS") << std::endl;
  return status ? 1 : 0;
}
