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
#include <island/gui/events.internal.hpp>

namespace {

using GUI::Properties;
using namespace INPUT::KEYS;

constexpr Float LARGE = 10.0f;

auto value(const Properties &properties) -> Float { return properties.value; }

auto least(const Properties &properties) -> Float {
  return properties.most > properties.least ? properties.least
                                            : properties.value;
}

auto most(const Properties &properties) -> Float {
  return properties.most > properties.least ? properties.most
                                            : properties.value;
}

auto resting(const Properties &properties) -> Float {
  return properties.resting;
}

struct Key {
  Action action;
  auto (*from)(const Properties &) -> Float;
  Float steps = 0.0f;
};

constexpr Float UNIT = 1.0f;

constexpr Key KEYS[] = {
  {RIGHT, ::value, UNIT}, {UP, ::value, UNIT},     {LEFT, ::value, -UNIT},
  {DOWN, ::value, -UNIT}, {PRIOR, ::value, LARGE}, {NEXT, ::value, -LARGE},
  {HOME, ::least},        {END, ::most},           {DELETE, ::resting}};

}  // namespace

auto GUI::turn(
  Handle document, Document &entry, Whole index,
  const INPUT::KEYS::Event &event) -> Flag {
  const auto &properties = entry.nodes[index].properties;
  for (const auto &key : ::KEYS) {
    if (key.action != event.action) continue;
    turn(
      document, entry, index,
      key.from(properties) + key.steps * step(properties));
    return true;
  }
  return false;
}
