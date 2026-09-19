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
#include <cstdio>
#include <iostream>
#include <utility>
#include <test/gui/gui.internal.hpp>

static auto number(Float value) -> String {
  constexpr Whole WIDTH = 32;
  Byte text[WIDTH] = {};
  std::snprintf(text, sizeof(text), "%g", static_cast<double>(value));
  return text;
}

static void hex(Float channel, String &text) {
  static constexpr STRING::Hot DIGITS = "0123456789abcdef";
  constexpr Float SCALE = 255.0f;
  constexpr Whole BASE = 16;
  const auto value = static_cast<Whole>(channel * SCALE + 0.5f);
  text += DIGITS[value / BASE];
  text += DIGITS[value % BASE];
}

static auto shade(const GFX::Color &color) -> String {
  String text = "#";
  hex(color.r, text);
  hex(color.g, text);
  hex(color.b, text);
  hex(color.a, text);
  return text;
}

static auto placed(GUI::Handle document, STRING::Hot id) -> String {
  const auto position = GUI::GET::position(document, id);
  const auto extent = GUI::GET::extent(document, id);
  return " x=" + number(position.x) + " y=" + number(position.y) +
         " w=" + number(extent.w) + " h=" + number(extent.h) +
         " fraction=" + number(GUI::GET::fraction(document, id)) +
         " z=" + number(GUI::GET::depth(document, id));
}

static auto anchored(GUI::Handle document, STRING::Hot id) -> String {
  static constexpr STRING::Hot COMPASS[] = {
    "northwest", "northeast", "southwest", "southeast", "center"};
  const auto pins = GUI::GET::stretch(document, id);
  if (pins == 0) return COMPASS[GUI::GET::anchor(document, id)];
  if (pins == GUI::EDGE::FILL) return "stretch";
  const std::pair<Whole, STRING::Hot> EDGES[] = {
    {GUI::EDGE::LEFT, "left"},
    {GUI::EDGE::RIGHT, "right"},
    {GUI::EDGE::TOP, "top"},
    {GUI::EDGE::BOTTOM, "bottom"}};
  String out;
  for (const auto &[flag, name] : EDGES)
    if (pins & flag) out += (out.empty() ? "" : "|") + String(name);
  return out;
}

static auto drawn(GUI::Handle document, STRING::Hot id) -> String {
  const auto frame = GUI::GET::frame(document, id);
  return String(" anchor=") + anchored(document, id) +
         " color=" + shade(GUI::GET::color(document, id)) + " text=\"" +
         GUI::GET::text(document, id) + "\" frame=" + number(frame.u) + "," +
         number(frame.v) + "," + number(frame.w) + "," + number(frame.h) +
         " visible=" + (GUI::GET::visibility(document, id) ? "true" : "false");
}

static auto dressed(GUI::Handle document, STRING::Hot id) -> String {
  return " border=" + number(GUI::GET::border(document, id)) +
         " edge=" + shade(GUI::GET::edge(document, id)) +
         " hover=" + shade(GUI::GET::hover(document, id)) +
         " press=" + shade(GUI::GET::press(document, id)) +
         " pad=" + number(GUI::GET::pad(document, id)) +
         " wrap=" + std::to_string(GUI::GET::wrap(document, id)) +
         " open=" + (GUI::GET::opening(document, id) ? "true" : "false");
}

static auto sited(GUI::Handle document, STRING::Hot id) -> String {
  static constexpr STRING::Hot WORDS[] = {"below", "above", "east", "west"};
  const auto seat = GUI::GET::seat(document, id);
  if (seat.beside.empty()) return {};
  return " beside=" + String(seat.beside) +
         " side=" + String(WORDS[seat.side]) + " air=" + number(seat.air);
}

auto describe(GUI::Handle document, const GUI::Node &node) -> String {
  const auto &nodes = GUI::STATE::documents[document].nodes;
  const String parent =
    node.parent == GUI::PARENTLESS ? "" : nodes[node.parent].id;
  return String(node.id) + " kind=" + node.kind + " parent=" + parent +
         placed(document, node.id.c_str()) + drawn(document, node.id.c_str()) +
         dressed(document, node.id.c_str()) + sited(document, node.id.c_str());
}

void print(GUI::Handle document, const GUI::Node &node) {
  std::cout << describe(document, node) << std::endl;
}

auto lines(GUI::Handle document) -> Vector<String> {
  Vector<String> out;
  for (const auto &node : GUI::STATE::documents[document].nodes)
    if (node.live) out.push_back(describe(document, node));
  return out;
}
