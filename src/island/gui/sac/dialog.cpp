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
#include <island/gui/dialog.internal.hpp>
#include <island/gui/sac.hpp>
#include <island/input.hpp>
#include <logger.hpp>
#include <filesystem>

static const String category = "~/island/gui::";

namespace {
GUI::Handle live = GUI::NONE;
GUI::DIALOG::Mode kind = GUI::DIALOG::OPEN;
String buffer;
String standing;
String screen;
Vector<GUI::DIALOG::Entry> rows;

void stand(const String &path) {
  std::error_code ec;
  const String seed = path.empty() ? String(".") : path;
  standing =
    std::filesystem::is_directory(seed, ec) ? seed : GUI::DIALOG::up(seed);
  rows = GUI::DIALOG::walk(standing, screen);
}

void listing() {
  const Flag bare = rows.empty() || (rows.size() == 1 && rows[0].directory &&
                                     rows[0].name == "..");
  GUI::set(
    live, GUI::DIALOG::WHERE,
    GUI::Text{bare ? standing + "  (nothing to list)" : standing});
  GUI::set(
    live, GUI::DIALOG::LISTING, GUI::Rows{static_cast<Whole>(rows.size())});
  const Whole first = GUI::GET::first(live, GUI::DIALOG::LISTING);
  const Whole window = GUI::GET::window(live, GUI::DIALOG::LISTING);
  for (Whole at = 0; at < window; at += 1) {
    const Whole index = first + at;
    if (index >= rows.size()) break;
    const String id = String(GUI::DIALOG::LISTING) + "." + std::to_string(at);
    GUI::set(
      live, id.c_str(),
      GUI::Text{rows[index].name + (rows[index].directory ? "/" : "")});
  }
}

void fill(const String &path) {
  GUI::set(live, GUI::DIALOG::FIELD, GUI::Text{path});
  GUI::set(
    live, GUI::DIALOG::FIELD,
    GUI::Caret{
      static_cast<Whole>(path.size()), static_cast<Whole>(path.size())});
}

void land(Whole row) {
  constexpr Char SEPARATOR = '/';
  if (row >= rows.size()) return;
  const String landed = GUI::DIALOG::join(standing, rows[row].name);
  ::fill(
    !rows[row].directory || landed.back() == SEPARATOR ? landed
                                                       : landed + SEPARATOR);
}

void enter(Whole row) {
  constexpr Char SEPARATOR = '/';
  if (row >= rows.size()) return;
  const String landed = GUI::DIALOG::join(standing, rows[row].name);
  if (rows[row].directory) {
    ::stand(landed);
    GUI::set(live, GUI::DIALOG::LISTING, GUI::Cursor{0});
    GUI::set(live, GUI::DIALOG::LISTING, GUI::Scroll{0});
    ::fill(standing.back() == SEPARATOR ? standing : standing + SEPARATOR);
    return;
  }
  ::fill(landed);
}

void dismiss() {
  if (live != GUI::NONE) {
    GUI::claim(live, false);
    GUI::remove(live);
  }
  live = GUI::NONE;
}
}  // namespace

void GUI::DIALOG::open(Mode mode, STRING::Hot path, STRING::Hot filter) {
  if (!GUI::SAC::GET::adopted(NAME)) {
    LOGGER::get(category + "dialog")
      .error(
        "The dialog is not designated: add `dialog: true` under the "
        "manifest's `gui:` block (or adopt it through GUI::SAC::adopt).");
    return;
  }
  raise(mode, path, filter);
}

void GUI::DIALOG::raise(Mode mode, STRING::Hot path, STRING::Hot filter) {
  dismiss();
  kind = mode;
  buffer = path ? String(path) : String();
  screen = filter ? String(filter) : String();
  ::stand(buffer);
  live = compose(kind, buffer);
  ::listing();
  GUI::edit(live, FIELD);
  const Whole end = buffer.size();
  GUI::set(live, FIELD, Caret{end, end});
}

auto GUI::DIALOG::place(GFX::Viewport viewport) -> Status {
  return live == NONE ? 1 : GUI::place(live, viewport);
}

auto GUI::DIALOG::poll() -> Result {
  if (live == NONE) return INACTIVE;
  GUI::focus(live, true);
  GUI::claim(live, true);
  GUI::poll();
  if (GUI::GET::selected(live, LISTING))
    ::land(GUI::GET::cursor(live, LISTING));
  if (GUI::GET::activated(live, LISTING))
    ::enter(GUI::GET::cursor(live, LISTING));
  ::listing();
  buffer = GUI::GET::text(live, FIELD);
  const Flag accept =
    GUI::GET::committed(live, FIELD) || GUI::GET::clicked(live, OK);
  const Flag cancel =
    INPUT::GET::pressed(INPUT::KEYS::ESCAPE) || GUI::GET::clicked(live, CANCEL);
  if (accept) return dismiss(), ACCEPTED;
  if (cancel) return dismiss(), CANCELLED;
  return PENDING;
}

void GUI::DIALOG::flush(GFX::Pass &pass) {
  if (live != NONE) GUI::flush(live, pass);
}

void GUI::DIALOG::close() { dismiss(); }

auto GUI::DIALOG::raised() -> Handle { return live; }

auto GUI::DIALOG::GET::active() -> Flag { return live != NONE; }

auto GUI::DIALOG::GET::path() -> String { return buffer; }

auto GUI::DIALOG::GET::mode() -> Mode { return kind; }
