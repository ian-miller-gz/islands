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
#include <island/splash/splash.internal.hpp>
#include <cartridge.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/windows.hpp>
#include <island/input.hpp>
#include <island/window/backend.hpp>

namespace {
GUI::Handle chrome = GUI::NONE;
Flag holding = false;
Flag lowered = false;
Float clicked = 0;
Whole polled = 0;
constexpr Float DOUBLE = 0.4f;
constexpr STRING::Hot SWOLLEN = "❐";
constexpr STRING::Hot PLAIN = "□";

auto yielded() -> Flag {
  const auto &pointer = INPUT::GET::pointer();
  return GUI::GET::under(pointer.x, pointer.y, chrome);
}

void tap() {
  const Float now = CLOCK::GET::elapsed();
  const Flag second = now - clicked < DOUBLE;
  clicked = now;
  if (second) WINDOW::maximize(!WINDOW::GET::maximized());
}

void react() {
  Flag flipping = false;
  for (const auto &event : GUI::GET::events(chrome)) {
    if (
      event.kind == GUI::Event::PRESSED && event.id == SPLASH::BAND &&
      !WINDOW::GET::maximized() && !::yielded()) {
      holding = true;
      WINDOW::drag(true);
    }
    if (event.kind != GUI::Event::CLICKED) continue;
    if (event.id == SPLASH::SHADE) flipping = true;
    if (event.id == SPLASH::SHRINK) WINDOW::minimize();
    if (event.id == SPLASH::SWELL) WINDOW::maximize(!WINDOW::GET::maximized());
    if (event.id == SPLASH::LEAVE) WINDOW::close();
    if (event.id == SPLASH::BAND) ::tap();
  }
  if (flipping)
    GUI::theme(GUI::GET::theme() == GUI::DARK ? GUI::LIGHT : GUI::DARK);
}

void carry() {
  if (!holding || INPUT::GET::pointer().left) return;
  holding = false;
  WINDOW::drag(false);
}

void priority() {
  const String over = GUI::GET::hover(chrome);
  const Flag taken = !over.empty() && (over != SPLASH::BAND || !::yielded());
  if (taken && GUI::GET::claimed() == GUI::NONE) GUI::claim(chrome, true);
  if (!taken && !holding) GUI::claim(chrome, false);
}

void refresh() {
  GUI::set(
    chrome, SPLASH::SWELL,
    GUI::Text{WINDOW::GET::maximized() ? SWOLLEN : PLAIN});
  if (WINDOW::GET::fullscreen() == lowered) return;
  lowered = !lowered;
  GUI::set(chrome, SPLASH::SHELL, GUI::Visibility{!lowered});
}

void graft() {
  auto &pass = GFX::PASSES::last();
  if (!pass.viewport.w) {
    pass.viewport.w = static_cast<Float>(GFX::WINDOWS::MAIN::width);
    pass.viewport.h = static_cast<Float>(GFX::WINDOWS::MAIN::height);
  }
  GUI::flush(chrome, pass);
}
}  // namespace

void SPLASH::prepare() {
  if (CARTRIDGE::splash) compose();
}

void SPLASH::render() {
  if (!CARTRIDGE::splash) return;
  ::chrome = GUI::GET::document(SOURCE);
  if (::chrome == GUI::NONE) ::chrome = compose();
  if (::chrome == GUI::NONE) return;
  if (GUI::GET::polls() == ::polled) GUI::poll();
  ::polled = GUI::GET::polls();
  ::react();
  ::carry();
  ::priority();
  ::refresh();
  GUI::place(::chrome, GFX::WINDOWS::MAIN::viewport());
  ::graft();
}
