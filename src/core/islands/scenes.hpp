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
#include <islands/select.hpp>

namespace ISLANDS::SCENES {

struct Scene {
  String name;
  void (*enter)() = nullptr;
  void (*update)() = nullptr;
  void (*render)() = nullptr;
};

struct Stage {
  Vector<Scene> scenes;
  Whole current = SELECT::NONE;
};

inline void add(Stage &stage, const Scene &scene) {
  stage.scenes.push_back(scene);
}

inline auto find(const Stage &stage, const String &name) -> Whole {
  for (Whole index = 0; index < stage.scenes.size(); index += 1)
    if (stage.scenes[index].name == name) return index;
  return SELECT::NONE;
}

inline auto enter(Stage &stage, const String &name) -> Flag {
  const Whole next = find(stage, name);
  if (next == SELECT::NONE) return false;
  stage.current = next;
  if (stage.scenes[next].enter) stage.scenes[next].enter();
  return true;
}

inline void update(Stage &stage) {
  if (stage.current != SELECT::NONE && stage.scenes[stage.current].update)
    stage.scenes[stage.current].update();
}

inline void render(Stage &stage) {
  if (stage.current != SELECT::NONE && stage.scenes[stage.current].render)
    stage.scenes[stage.current].render();
}

}  // namespace ISLANDS::SCENES
