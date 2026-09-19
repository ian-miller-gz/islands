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
#include <test/gui/gui.internal.hpp>

static Status write(GUI::Handle document) {
  using namespace GUI;
  Status status = check(
    "position", set(document, "title", Position{4.0f, 2.0f}) == 0 &&
                  GET::position(document, "title").x == 4.0f &&
                  GET::position(document, "title").y == 2.0f);
  status |= check(
    "extent", set(document, "title", Extent{32.0f, 8.0f}) == 0 &&
                GET::extent(document, "title").h == 8.0f);
  status |= check(
    "color", set(document, "title", GFX::COLORS::BLACK) == 0 &&
               GET::color(document, "title").r == 0.0f &&
               GET::color(document, "title").a == 1.0f);
  status |= check(
    "text", set(document, "title", Text{"Hello"}) == 0 &&
              GET::text(document, "title") == "Hello");
  status |= check(
    "fraction", set(document, "title", Fraction{0.5f}) == 0 &&
                  GET::fraction(document, "title") == 0.5f);
  status |= check(
    "visibility", set(document, "title", Visibility{false}) == 0 &&
                    !GET::visibility(document, "title"));
  return status;
}

static Status chrome(GUI::Handle document) {
  using namespace GUI;
  Status status = check(
    "border", set(document, "title", Border{2.0f}) == 0 &&
                GET::border(document, "title") == 2.0f);
  status |= check(
    "edge", set(document, "title", Edge{GFX::COLORS::BLACK}) == 0 &&
              GET::edge(document, "title").r == 0.0f);
  status |= check(
    "hover", set(document, "title", Hover{GFX::COLORS::BLACK}) == 0 &&
               GET::hover(document, "title").a == 1.0f);
  status |= check(
    "press", set(document, "title", Press{GFX::COLORS::BLACK}) == 0 &&
               GET::press(document, "title").g == 0.0f);
  status |= check("unglowed", GET::glow(document, "title").a == 0.0f);
  status |= check(
    "glow", set(document, "title", Glow{GFX::COLORS::WHITE}) == 0 &&
              GET::glow(document, "title").a == 1.0f);
  status |= check(
    "pad", set(document, "title", Pad{6.0f}) == 0 &&
             GET::pad(document, "title") == 6.0f);
  status |= check(
    "wrap", set(document, "title", Wrap{24}) == 0 &&
              GET::wrap(document, "title") == 24);
  bind("icons", 0);
  status |= check(
    "image", set(document, "title", Image{"icons", 2, 3}) == 0 &&
               GET::frame(document, "title").u == 2.0f &&
               GET::frame(document, "title").v == 3.0f);
  status |= check(
    "refuse unbound atlas", set(document, "title", Image{"ghost", 0, 0}) != 0);
  return status;
}

static Status miss(GUI::Handle document) {
  using namespace GUI;
  Status status = check(
    "refuse unknown id", set(document, "ghost", Position{1.0f, 1.0f}) != 0);
  status |= check(
    "read unknown id as zero", GET::position(document, "ghost").x == 0.0f &&
                                 GET::text(document, "ghost").empty() &&
                                 !GET::visibility(document, "ghost"));
  return status;
}

Status state(GUI::Handle document) {
  return write(document) | chrome(document) | miss(document);
}
