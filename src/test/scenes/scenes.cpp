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
#include <iostream>
#include <islands/scenes.hpp>

namespace SCENES = ISLANDS::SCENES;
namespace SELECT = ISLANDS::SELECT;

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

struct Counts {
  Whole enter = 0, update = 0, render = 0;
};
Counts menu, play, over;

void menuEnter() { menu.enter += 1; }
void menuUpdate() { menu.update += 1; }
void menuRender() { menu.render += 1; }
void playEnter() { play.enter += 1; }
void playUpdate() { play.update += 1; }
void playRender() { play.render += 1; }
void overEnter() { over.enter += 1; }

auto build() -> SCENES::Stage {
  SCENES::Stage stage;
  SCENES::add(stage, {"menu", menuEnter, menuUpdate, menuRender});
  SCENES::add(stage, {"play", playEnter, playUpdate, playRender});
  SCENES::add(stage, {"over", overEnter, nullptr, nullptr});
  return stage;
}

}  // namespace

auto main() -> int {
  SCENES::Stage stage = build();
  Status status = 0;

  status |= check("registered 3", stage.scenes.size() == 3);
  status |= check("current NONE at start", stage.current == SELECT::NONE);
  status |= check("find play = 1", SCENES::find(stage, "play") == 1);
  status |=
    check("find unknown = NONE", SCENES::find(stage, "boss") == SELECT::NONE);

  status |= check("enter menu ok", SCENES::enter(stage, "menu"));
  status |=
    check("enter fired menu.enter", menu.enter == 1 && stage.current == 0);
  SCENES::update(stage);
  SCENES::render(stage);
  status |= check(
    "walk drove menu only",
    menu.update == 1 && menu.render == 1 && play.update == 0);

  status |= check("enter play ok", SCENES::enter(stage, "play"));
  SCENES::update(stage);
  SCENES::render(stage);
  status |= check(
    "walk drove play, not menu again",
    play.enter == 1 && play.update == 1 && menu.update == 1);

  status |= check("enter unknown refused", !SCENES::enter(stage, "boss"));
  status |= check("refused left current at play", stage.current == 1);

  status |= check("enter over ok", SCENES::enter(stage, "over"));
  SCENES::update(stage);
  SCENES::render(stage);
  status |= check("null-hook walk safe", over.enter == 1);

  return status;
}
