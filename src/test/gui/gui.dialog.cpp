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
#include <iostream>
#include <test/gui/gui.internal.hpp>

namespace {
using GUI::DIALOG::Result;

constexpr Float OK_X = 490.0f;
constexpr Float CANCEL_X = 402.0f;
constexpr Float BUTTON_Y = 397.0f;
constexpr Float ELSEWHERE = 10.0f;

auto name(Result result) -> STRING::Hot {
  static constexpr STRING::Hot NAMES[] = {
    "inactive", "pending", "accepted", "cancelled"};
  return NAMES[result];
}

void beat(STRING::Hot name) {
  GUI::DIALOG::place(WHOLE);
  const Result result = GUI::DIALOG::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  if (GUI::DIALOG::GET::active()) GUI::DIALOG::flush(pass);
  std::cout << "beat " << name << ": result=" << ::name(result)
            << " active=" << GUI::DIALOG::GET::active() << " path='"
            << GUI::DIALOG::GET::path() << "'" << std::endl;
}

auto letters(STRING::Hot text) -> Vector<INPUT::KEYS::Event> {
  Vector<INPUT::KEYS::Event> events;
  for (const char *at = text; *at; at += 1)
    events.push_back({INPUT::KEYS::TEXT, static_cast<Whole>(*at)});
  return events;
}
}  // namespace

Status picker() {
  using namespace GUI;
  SAC::adopt({DIALOG::NAME});
  rescale(1.0f);
  aim(ELSEWHERE, ELSEWHERE);
  hold(false);
  DIALOG::open(DIALOG::OPEN, "");
  beat("open");
  typed(letters("notes.txt"));
  beat("type");
  aim(OK_X, BUTTON_Y);
  hold(true);
  beat("press");
  hold(false);
  beat("accept");
  DIALOG::open(DIALOG::SAVE, "draft.md");
  aim(ELSEWHERE, ELSEWHERE);
  beat("save");
  typed({{INPUT::KEYS::ERASE}, {INPUT::KEYS::ERASE}});
  beat("erase");
  aim(CANCEL_X, BUTTON_Y);
  hold(true);
  beat("grab");
  hold(false);
  beat("dismiss");
  DIALOG::open(DIALOG::OPEN, "x");
  aim(ELSEWHERE, ELSEWHERE);
  beat("reopen");
  tap(INPUT::KEYS::ESCAPE);
  beat("escape");
  return 0;
}
