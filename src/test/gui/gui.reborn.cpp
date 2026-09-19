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
#include <filesystem>
#include <iostream>
#include <test/gui/gui.internal.hpp>

namespace {

GUI::Handle sheet = GUI::NONE;

constexpr STRING::Hot SOURCE = "build/reborn.gui";
constexpr STRING::Hot LAYOUT =
  "panel deck x=10 y=10 w=200 h=120\n"
  "  label title x=10 y=10 w=180 h=20 text=\"authored\"\n";

constexpr STRING::Hot HUNG = "the owner's own";

void look(STRING::Hot name) {
  std::cout << "reborn " << name
            << ": generation=" << GUI::GET::generation(::sheet)
            << " authored=\"" << GUI::GET::text(::sheet, "title")
            << "\" hung=\"" << GUI::GET::text(::sheet, "hung") << "\""
            << std::endl;
}

Status compose() {
  Status status =
    check("hang", GUI::NODES::create(::sheet, "deck", "label", "hung") == 0);
  return status |
         check("write", GUI::set(::sheet, "hung", GUI::Text{::HUNG}) == 0);
}

Status author() {
  std::error_code error;
  std::filesystem::create_directories(
    std::filesystem::path(::SOURCE).parent_path(), error);
  IO::STREAMS::Output file(::SOURCE);
  file << ::LAYOUT;
  file.close();
  return check("author", std::filesystem::exists(::SOURCE));
}

Status begin(Whole &born) {
  Status status = ::author();
  ::sheet = GUI::load(::SOURCE);
  status |= check("load", ::sheet != GUI::NONE);
  born = GUI::GET::generation(::sheet);
  status |= check("born", born != GUI::UNBORN);
  ::look("loaded");
  status |= ::compose();
  ::look("composed");
  return status | check("still born", GUI::GET::generation(::sheet) == born);
}

Status flip(Whole born) {
  GUI::theme(GUI::DARK);
  ::look("dark");
  Status status = check("kept", GUI::live(::sheet));
  status |= check("bumped", GUI::GET::generation(::sheet) == born + 1);
  status |=
    check("authored survived", GUI::GET::text(::sheet, "title") == "authored");
  status |= check("hung gone", GUI::GET::text(::sheet, "hung").empty());
  return status |
         check(
           "write refused", GUI::set(::sheet, "hung", GUI::Text{::HUNG}) != 0);
}

Status recover(Whole born) {
  Status status = ::compose();
  ::look("re-composed");
  status |= check("recovered", GUI::GET::text(::sheet, "hung") == ::HUNG);
  GUI::theme(GUI::LIGHT);
  ::look("light");
  return status |
         check("bumped again", GUI::GET::generation(::sheet) == born + 2);
}

Status stranger() {
  Status status =
    check("a stranger is unborn", GUI::GET::generation(GUI::NONE) == 0);
  GUI::remove(::sheet);
  ::look("removed");
  return status |
         check("the dropped are unborn", GUI::GET::generation(::sheet) == 0);
}

}  // namespace

Status reborn() {
  Whole born = GUI::UNBORN;
  Status status = ::begin(born);
  status |= ::flip(born);
  status |= ::recover(born);
  return status | ::stranger();
}
