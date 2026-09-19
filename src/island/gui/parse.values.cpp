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
#include <cstdlib>
#include <island/gui/parse.internal.hpp>

auto GUI::PARSE::read(const String &value, Float &out) -> Status {
  Byte *end = nullptr;
  out = std::strtof(value.c_str(), &end);
  return value.empty() || *end != '\0';
}

auto GUI::PARSE::read(const String &value, Whole &out) -> Status {
  constexpr int DECIMAL = 10;
  Byte *end = nullptr;
  out = static_cast<Whole>(std::strtoul(value.c_str(), &end, DECIMAL));
  return value.empty() || *end != '\0' || value.front() == '-';
}

auto GUI::PARSE::read(const String &value, Flag &out) -> Status {
  out = value == "true";
  return !out && value != "false";
}

auto GUI::PARSE::read(const String &value, GFX::Color &out) -> Status {
  static constexpr STRING::Hot DIGITS = "0123456789abcdefABCDEF";
  constexpr Whole CHANNELS = 4;
  constexpr Whole PAIR = 2;
  constexpr int HEX = 16;
  constexpr Float SCALE = 255.0f;
  if (value.size() != 1 + CHANNELS * PAIR || value.front() != '#') return 1;
  if (value.find_first_not_of(DIGITS, 1) != String::npos) return 1;
  Float *channels[CHANNELS] = {&out.r, &out.g, &out.b, &out.a};
  for (Whole index = 0; index < CHANNELS; index += 1)
    *channels[index] =
      static_cast<Float>(std::strtoul(
        value.substr(1 + index * PAIR, PAIR).c_str(), nullptr, HEX)) /
      SCALE;
  return 0;
}

auto GUI::PARSE::read(const String &value, Anchor &out) -> Status {
  static const Map<String, Anchor> COMPASS = {
    {"northwest", NORTHWEST},
    {"northeast", NORTHEAST},
    {"southwest", SOUTHWEST},
    {"southeast", SOUTHEAST},
    {"center", CENTER}};
  const auto found = COMPASS.find(value);
  if (found == COMPASS.end()) return 1;
  out = found->second;
  return 0;
}

auto GUI::PARSE::read(const String &value, Side &out) -> Status {
  static constexpr STRING::Hot WORDS[] = {"below", "above", "east", "west"};
  for (Whole at = 0; at < sizeof(WORDS) / sizeof(*WORDS); at += 1) {
    if (value != WORDS[at]) continue;
    out = static_cast<Side>(at);
    return 0;
  }
  return 1;
}

auto GUI::PARSE::read(const String &value, Axis &out) -> Status {
  if (value != "across" && value != "down" && value != "free") return 1;
  out.free = value == "free";
  out.across = out.free || value == "across";
  return 0;
}

auto GUI::PARSE::read(const String &value, Carry &out) -> Status {
  if (value != "frame" && value != "pointer") return 1;
  out.pointer = value == "pointer";
  return 0;
}

auto GUI::PARSE::read(const String &value, Shape &out) -> Status {
  static constexpr STRING::Hot WORDS[] = {
    "line", "rise", "fall", "ease", "hold"};
  for (Whole at = 0; at < sizeof(WORDS) / sizeof(*WORDS); at += 1) {
    if (value != WORDS[at]) continue;
    out.run = static_cast<Shape::Run>(at);
    return 0;
  }
  return 1;
}

auto GUI::PARSE::read(const String &value, Stretch &out) -> Status {
  static const Map<String, Whole> EDGES = {
    {"stretch", EDGE::FILL},
    {"left", EDGE::LEFT},
    {"right", EDGE::RIGHT},
    {"top", EDGE::TOP},
    {"bottom", EDGE::BOTTOM}};
  out.edges = 0;
  String::size_type begin = 0;
  while (begin <= value.size()) {
    const auto bar = value.find('|', begin);
    const auto token = value.substr(begin, bar - begin);
    const auto found = EDGES.find(token);
    if (found == EDGES.end()) return 1;
    out.edges |= found->second;
    if (bar == String::npos) break;
    begin = bar + 1;
  }
  return 0;
}

auto GUI::PARSE::read(const String &value, GFX::SPRITES::Frame &out) -> Status {
  constexpr Whole CELLS = 4;
  Float *cells[CELLS] = {&out.u, &out.v, &out.w, &out.h};
  String::size_type begin = 0;
  for (Whole index = 0; index < CELLS; index += 1) {
    const auto end = value.find(',', begin);
    if ((end == String::npos) != (index == CELLS - 1)) return 1;
    if (read(value.substr(begin, end - begin), *cells[index]) != 0) return 1;
    begin = end + 1;
  }
  return 0;
}
