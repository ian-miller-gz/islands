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
#include <fstream>
#include <iostream>
#include <test/gui/gui.internal.hpp>
#include <island/gui/dialog.internal.hpp>

namespace fs = std::filesystem;

namespace {

using GUI::DIALOG::Result;

fs::path root;

auto plant() -> Flag {
  std::error_code ec;
  ::root = fs::temp_directory_path(ec) / "islands-browse";
  fs::remove_all(::root, ec);
  fs::create_directories(::root / "alpha", ec);
  fs::create_directories(::root / "beta", ec);
  for (const auto &path :
       {::root / "2-early.gui", ::root / "10-late.gui", ::root / "note.md",
        ::root / "alpha" / "inner.gui"}) {
    std::ofstream file(path);
    if (!file) return false;
  }
  return !ec;
}

auto shown(const String &path) -> String {
  if (path.empty()) return "-";
  const auto here = fs::path(path).lexically_relative(::root);
  return here.empty() ? path : here.string();
}

void beat(STRING::Hot name) {
  GUI::DIALOG::place(WHOLE);
  const Result result = GUI::DIALOG::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  const Flag active = GUI::DIALOG::GET::active();
  if (active) GUI::DIALOG::flush(pass);
  std::cout << "browse " << name << ": rows=[";
  if (active) {
    const auto document = GUI::DIALOG::raised();
    const auto rows = GUI::GET::rows(document, GUI::DIALOG::LISTING);
    const auto first = GUI::GET::first(document, GUI::DIALOG::LISTING);
    const auto window = GUI::GET::window(document, GUI::DIALOG::LISTING);
    STRING::Hot comma = "";
    for (Whole at = 0; at < window && first + at < rows; at += 1) {
      const String id = String(GUI::DIALOG::LISTING) + "." + std::to_string(at);
      std::cout << comma << GUI::GET::text(document, id.c_str());
      comma = " ";
    }
    std::cout << "] cursor="
              << GUI::GET::cursor(document, GUI::DIALOG::LISTING);
  } else {
    std::cout << "] cursor=-";
  }
  std::cout << " path=" << ::shown(GUI::DIALOG::GET::path()) << " result="
            << (result == Result::ACCEPTED    ? "accepted"
                : result == Result::CANCELLED ? "cancelled"
                : result == Result::PENDING   ? "pending"
                                              : "inactive")
            << std::endl;
}

void press(INPUT::KEYS::Action action) { typed({{action}}); }

void press(INPUT::KEYS::Action first, INPUT::KEYS::Action second) {
  typed({{first}, {second}});
}

}  // namespace

Status browse() {
  using namespace GUI;
  SAC::adopt({DIALOG::NAME});
  rescale(1.0f);
  aim(0.0f, 0.0f);
  hold(false);
  Status status = check("browse plant", ::plant());
  DIALOG::open(DIALOG::OPEN, ::root.string().c_str());
  ::beat("stand");
  ::press(INPUT::KEYS::DOWN, INPUT::KEYS::DOWN);
  ::beat("onto");
  ::press(INPUT::KEYS::UP, INPUT::KEYS::ENTER);
  ::beat("into");
  ::press(INPUT::KEYS::DOWN, INPUT::KEYS::ENTER);
  ::beat("picked");
  ::press(INPUT::KEYS::HOME, INPUT::KEYS::ENTER);
  ::beat("out");
  DIALOG::close();
  DIALOG::open(DIALOG::OPEN, ::root.string().c_str(), ".gui");
  ::beat("screened");
  DIALOG::close();
  status |= check(
    "a missing directory lists nothing",
    DIALOG::walk((::root / "nowhere").string(), "").empty());
  DIALOG::open(DIALOG::OPEN, (::root / "note.md").string().c_str());
  ::beat("seeded");
  GUI::edit(DIALOG::raised(), DIALOG::FIELD);
  ::press(INPUT::KEYS::ENTER);
  ::beat("confirm");
  std::error_code ec;
  fs::remove_all(::root, ec);
  return status;
}
