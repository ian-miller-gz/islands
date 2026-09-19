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
#include <test/gui/gui.internal.hpp>

namespace {

GUI::Handle bench = GUI::NONE;

constexpr Float LINE = 16.0f;
constexpr Float ADVANCE = 8.0f;
constexpr Float AUTHORED = 32.0f;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=280 h=180\n"
  "  detail sign x=10 y=10 w=80 h=20 size=32 pad=4 wrap=8 open=true"
  " text=\"two words here\"\n"
  "  detail fold x=10 y=90 w=80 h=20 size=32 pad=4 open=true"
  " text=\"two words here\"\n"
  "  label mark x=140 y=10 w=60 h=20 size=32 text=\"mark\"\n"
  "column tray x=8 y=210 w=120 h=80\n"
  "  detail note w=80 h=20 size=32 pad=4 wrap=8 open=true"
  " text=\"two words here\"\n";

void where(STRING::Hot id) {
  const auto size = GUI::GET::measured(::bench, id);
  std::cout << " " << id << " x" << size.w << "," << size.h;
}

void look(STRING::Hot name) {
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  std::cout << std::fixed << std::setprecision(1) << "letters " << name << ":";
  ::where("sign");
  ::where("fold");
  ::where("mark");
  ::where("note");
  std::cout << std::endl;
}

auto tall(STRING::Hot id) -> Float { return GUI::GET::measured(::bench, id).h; }

}  // namespace

Status letters() {
  const String layout = LAYOUT;
  rescale(1.0f);
  metrics(::LINE, ::ADVANCE);
  ::bench = GUI::load("letters", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("letters load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::look("rest");
  const Float lines = 3.0f;
  const Float rim = 2.0f * 4.0f;
  status |= check("authored fold", ::tall("sign") == rim + ::LINE * lines);
  status |=
    check("size is the basis", GUI::GET::size(::bench, "sign") == ::AUTHORED);
  const Float sign = ::tall("sign");
  const Float fold = ::tall("fold");
  const Float note = ::tall("note");
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::look("zoomed");
  status |= check("the text rides the zoom", ::tall("sign") == 2.0f * sign);
  status |=
    check("the derived fold rides it too", ::tall("fold") == 2.0f * fold);
  status |= check("the fold stands", ::tall("fold") / 2.0f == fold);
  status |= check("the stacked size stands", ::tall("note") == note);
  status |=
    check("shrink", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{0.5f}) == 0);
  ::look("shrunk");
  status |= check("the text shrinks with it", ::tall("sign") == sign / 2.0f);
  status |=
    check("and so does the derived fold", ::tall("fold") == fold / 2.0f);
  status |= check("the stacked size still stands", ::tall("note") == note);
  return status;
}
