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
#include <iomanip>
#include <iostream>
#include <island/gui/translate.hpp>
#include <test/gui/gui.internal.hpp>

namespace {

GUI::Handle bench = GUI::NONE;

constexpr Float LINE = 16.0f;
constexpr Float ADVANCE = 8.0f;
constexpr Float AUTHORED = 32.0f;

constexpr STRING::Hot WIDGETS =
  "graph deck x=20 y=20 w=280 h=180 size=32\n"
  "  node osc x=10 y=10 w=80 h=40 text=\"osc\"\n"
  "    label osc.out x=60 y=14 w=16 h=12 port=out\n"
  "  node amp x=140 y=10 w=80 h=40 text=\"amp\"\n"
  "    label amp.in x=0 y=14 w=16 h=12 port=in\n"
  "  node out x=140 y=100 w=80 h=40 port=in text=\"out\"\n";
constexpr STRING::Hot JOINS =
  "link osc.out amp.in \"excites\"\n"
  "link amp out\n";

constexpr STRING::Hot BROKEN[][2] = {
  {"refuse half a pair", "link osc.out\n"},
  {"refuse a stranger", "link osc.out ghost\n"},
  {"refuse a self join", "link osc.out osc.out\n"},
  {"refuse a nested line", "  link osc.out amp.in\n"},
  {"refuse a second label", "link osc.out amp.in \"a\" \"b\"\n"}};

auto written(GUI::Handle document) -> String {
  String text;
  return GUI::TRANSLATE::emit(document, text) == 0 ? text : String();
}

void quoted(const String &text) {
  String line;
  for (const Char letter : text) {
    if (letter != '\n') {
      line += letter;
      continue;
    }
    if (line.starts_with("link ")) std::cout << "saved emit: " << line << "\n";
    line.clear();
  }
  std::cout << std::flush;
}

auto refuses(STRING::Hot name, STRING::Hot line) -> Flag {
  const String source = String(WIDGETS) + line;
  const auto document =
    GUI::load(name, Vector<Byte>(source.begin(), source.end()));
  if (document != GUI::NONE) GUI::remove(document);
  return document == GUI::NONE;
}

void drew(STRING::Hot name) {
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  std::cout << std::fixed << std::setprecision(1) << "saved " << name << ":";
  for (const auto &run : inked())
    std::cout << " " << run.text << "@" << run.x << "," << run.y << " x"
              << run.scale;
  std::cout << std::endl;
}

}  // namespace

Status saved() {
  const String source = String(WIDGETS) + JOINS;
  rescale(1.0f);
  metrics(::LINE, ::ADVANCE);
  ::bench = GUI::load("saved", Vector<Byte>(source.begin(), source.end()));
  Status status = check("saved load", ::bench != GUI::NONE);
  status |= check("joins read", GUI::NGA::GET::links(::bench, "").size() == 2);
  status |= check(
    "label read",
    GUI::NGA::GET::label(::bench, "osc.out", "amp.in") == "excites");
  status |=
    check("bare label", GUI::NGA::GET::label(::bench, "amp", "out") == "");
  status |=
    check("directed", !GUI::NGA::GET::linked(::bench, "amp.in", "osc.out"));
  const String first = ::written(::bench);
  ::quoted(first);
  const auto again =
    GUI::load("resaved", Vector<Byte>(first.begin(), first.end()));
  status |= check("canonical reloads", again != GUI::NONE);
  status |= check("byte stable", !first.empty() && first == ::written(again));
  status |= check("tree survives", lines(::bench) == lines(again));
  status |= check("joins survive", GUI::NGA::GET::links(again, "").size() == 2);
  status |= check(
    "label survives",
    GUI::NGA::GET::label(again, "osc.out", "amp.in") == "excites");
  GUI::remove(again);
  for (const auto &broken : BROKEN)
    status |= check(broken[0], ::refuses(broken[0], broken[1]));
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::drew("rest");
  status |=
    check("size is the basis", GUI::GET::size(::bench, "deck") == ::AUTHORED);
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::drew("zoomed");
  status |= check(
    "rename", GUI::NGA::connect(
                ::bench, "osc.out", "amp.in", GUI::NGA::Label{"drives"}) == 0);
  ::drew("renamed");
  status |= check(
    "disconnect", GUI::NGA::disconnect(::bench, "osc.out", "amp.in") == 0);
  ::drew("parted");
  return status;
}
