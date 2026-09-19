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
#include <island/gui/translate.hpp>
#include <island/gui/translate.internal.hpp>
#include <island/gui/gui.internal.hpp>
#include <island/gui/parse.internal.hpp>
#include <utility>

namespace {

namespace T = GUI::TRANSLATE;

auto picture(const GUI::Properties &properties, GFX::SPRITES::Frame frame)
  -> String {
  if (properties.atlas_name.empty())
    return " frame=" + T::number(frame.u) + "," + T::number(frame.v) + "," +
           T::number(frame.w) + "," + T::number(frame.h);
  const Whole column =
    frame.w > 0.0f ? static_cast<Whole>(frame.u / frame.w + 0.5f) : 0;
  const Whole row =
    frame.h > 0.0f ? static_cast<Whole>(frame.v / frame.h + 0.5f) : 0;
  return " frame=" + String(properties.atlas_name) + ":" +
         std::to_string(column) + "," + std::to_string(row);
}

auto anchored(GUI::Handle document, STRING::Hot id) -> String {
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
  for (const auto &[flag, word] : EDGES)
    if (pins & flag) out += (out.empty() ? "" : "|") + String(word);
  return out;
}

auto facing(const GUI::Properties &properties) -> String {
  static constexpr STRING::Hot WORDS[] = {"none", "in", "out"};
  const Whole side = properties.port;
  String token = side < sizeof(WORDS) / sizeof(*WORDS) ? WORDS[side] : WORDS[0];
  if (!properties.kind.empty()) token += "," + properties.kind;
  return token;
}

auto travelled(GUI::Axis axis) -> STRING::Hot {
  return axis.free ? "free" : axis.across ? "across" : "down";
}

auto axised(const GUI::Properties &properties) -> String {
  static constexpr STRING::Hot WORDS[] = {"none", "across", "down"};
  const Whole axis = properties.pinned;
  return String(axis < sizeof(WORDS) / sizeof(*WORDS) ? WORDS[axis] : WORDS[0]);
}

auto scaled(const GUI::Properties &properties) -> String {
  const auto zoom = properties.zoom;
  if (zoom.w == zoom.h) return T::number(zoom.w);
  return T::number(zoom.w) + "," + T::number(zoom.h);
}

auto sited(const GUI::Properties &properties) -> String {
  static constexpr STRING::Hot WORDS[] = {"below", "above", "east", "west"};
  if (properties.beside.empty()) return {};
  const Whole side = properties.side;
  return " beside=" + String(properties.beside) + " side=" +
         String(
           side < sizeof(WORDS) / sizeof(*WORDS) ? WORDS[side] : WORDS[0]) +
         " air=" + T::number(properties.air);
}

auto running(const GUI::Properties &properties) -> String {
  static constexpr STRING::Hot WORDS[] = {
    "line", "rise", "fall", "ease", "hold"};
  const Whole shape = properties.shape;
  const String word =
    shape < sizeof(WORDS) / sizeof(*WORDS) ? WORDS[shape] : WORDS[0];
  return " to=" + T::number(properties.to.x) + "," +
         T::number(properties.to.y) + " shape=" + word +
         " bend=" + T::number(properties.bend);
}

auto tokens(GUI::Handle document, const GUI::Node &node) -> String {
  const auto id = node.id.c_str();
  const auto position = GUI::GET::position(document, id);
  const auto extent = GUI::GET::extent(document, id);
  const auto frame = GUI::GET::frame(document, id);
  return " x=" + T::number(position.x) + " y=" + T::number(position.y) +
         " w=" + T::number(extent.w) + " h=" + T::number(extent.h) +
         " anchor=" + ::anchored(document, id) +
         " color=" + T::shade(GUI::GET::color(document, id)) + " text=\"" +
         GUI::GET::text(document, id) + "\"" +
         ::picture(node.properties, frame) +
         " fraction=" + T::number(GUI::GET::fraction(document, id)) +
         " z=" + T::number(GUI::GET::depth(document, id)) +
         " border=" + T::number(GUI::GET::border(document, id)) +
         " edge=" + T::shade(GUI::GET::edge(document, id)) +
         " hover=" + T::shade(GUI::GET::hover(document, id)) +
         " press=" + T::shade(GUI::GET::press(document, id)) +
         " glow=" + T::shade(GUI::GET::glow(document, id)) +
         " wash=" + T::shade(GUI::GET::wash(document, id)) +
         " pad=" + T::number(GUI::GET::pad(document, id)) +
         " wrap=" + std::to_string(GUI::GET::wrap(document, id)) +
         " size=" + T::number(GUI::GET::size(document, id)) +
         " open=" + (GUI::GET::opening(document, id) ? "true" : "false") +
         " visible=" + (GUI::GET::visibility(document, id) ? "true" : "false") +
         " clip=" + (GUI::GET::clipping(document, id) ? "true" : "false") +
         " rows=" + std::to_string(GUI::GET::rows(document, id)) +
         " cursor=" + std::to_string(GUI::GET::cursor(document, id)) +
         " first=" + std::to_string(GUI::GET::first(document, id)) +
         " pitch=" + T::number(GUI::GET::pitch(document, id)) +
         " mark=" + String(node.properties.mark) +
         " plain=" + String(node.properties.plain) +
         " pattern=" + String(node.properties.pattern) +
         " quick=" + (GUI::GET::quick(document, id) ? "true" : "false") +
         " scroll=" + (GUI::GET::scrolling(document, id) ? "true" : "false") +
         " axis=" + ::travelled(GUI::GET::axis(document, id)) +
         (node.properties.carried ? String(" carry=pointer") : String()) +
         " least=" + T::number(node.properties.least) +
         " most=" + T::number(node.properties.most) +
         " resting=" + T::number(node.properties.resting) +
         " steps=" + std::to_string(node.properties.steps) +
         " graphic=" + (node.properties.graphic ? "true" : "false") +
         " value=" + T::number(GUI::GET::value(document, id)) +
         " port=" + ::facing(node.properties) +
         " pinned=" + ::axised(node.properties) +
         (node.properties.banded ? String(" choosing=band") : String()) +
         " pan=" + T::number(node.properties.pan.x) + "," +
         T::number(node.properties.pan.y) +
         " zoom=" + ::scaled(node.properties) + ::sited(node.properties) +
         (node.kind == "stroke" ? ::running(node.properties) : String());
}

auto depth(const Vector<GUI::Node> &nodes, const GUI::Node &node) -> Whole {
  Whole level = 0;
  for (Whole at = node.parent; at != GUI::PARENTLESS; at = nodes[at].parent)
    level += 1;
  return level;
}

}  // namespace

auto GUI::TRANSLATE::emit(Handle document, String &text) -> Status {
  if (!GUI::live(document)) return 1;
  const auto &nodes = GUI::STATE::documents[document].nodes;
  String out;
  for (const auto &node : nodes) {
    if (!node.live) continue;
    out += String(GUI::PARSE::INDENT * ::depth(nodes, node), ' ') +
           String(node.kind) + " " + String(node.id) +
           ::tokens(document, node) + "\n";
  }
  text = out + T::joins(document);
  return 0;
}
