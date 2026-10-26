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
#include <island/gui/gui.internal.hpp>
#include <island/gui/parse.internal.hpp>
#include <island/gui/parse.inks.hpp>
#include <algorithm>

namespace {

constexpr GUI::PARSE::Ink TABLE[] = {
  {"color", &GUI::Properties::color}, {"edge", &GUI::Properties::edge},
  {"hover", &GUI::Properties::hover}, {"press", &GUI::Properties::press},
  {"glow", &GUI::Properties::glow},   {"wash", &GUI::Properties::wash},
  {"track", &GUI::Properties::track}};

auto take(const String &key, const String &value, GFX::Color &color)
  -> const GUI::PARSE::Ink * {
  const auto found = std::ranges::find(::TABLE, key, &GUI::PARSE::Ink::key);
  if (found == std::ranges::end(::TABLE)) return nullptr;
  return GUI::PARSE::read(value, color) == 0 ? found : nullptr;
}

}  // namespace

const std::span<const GUI::PARSE::Ink> GUI::PARSE::INKS = ::TABLE;

auto GUI::PARSE::inked(const String &key) -> Flag {
  return std::ranges::contains(::TABLE, key, &Ink::key);
}

auto GUI::PARSE::ink(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  Whole node = PARENTLESS;
  auto color = GFX::COLORS::WHITE;
  const auto found = ::take(key, value, color);
  if (found == nullptr || find(document, id, node) != 0) return 1;
  auto &field = STATE::documents[document].nodes[node].properties.*found->field;
  if (field == color) return 0;
  field = color;
  return mark(document, node);
}

auto GUI::PARSE::ink(
  Properties &properties, const String &key, const String &value) -> Status {
  auto color = GFX::COLORS::WHITE;
  const auto found = ::take(key, value, color);
  if (found != nullptr) properties.*found->field = color;
  return found == nullptr ? 1 : 0;
}
