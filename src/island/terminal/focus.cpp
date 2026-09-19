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
#include <island/input.hpp>
#include <island/overlay.hpp>
#include <island/terminal/focus.hpp>
#include <console.hpp>

static Whole focused = 0;

namespace {
auto holder() -> CONSOLES::Console * {
  auto &consoles = CONSOLES::GET::consoles();
  return focused < consoles.size() ? &consoles[focused] : nullptr;
}

void summon(Whole hotkey) {
  auto &consoles = CONSOLES::GET::consoles();
  auto *owner = holder();
  const Flag active = owner && owner->visible && owner->hotkey == hotkey;
  Whole next = consoles.size();
  for (Whole index = active ? focused + 1 : 0; index < consoles.size();
       index += 1)
    if (consoles[index].hotkey == hotkey) {
      next = index;
      break;
    }
  if (!active && next == consoles.size()) return;
  for (auto &other : consoles) other.visible = false;
  focused = next;
  if (auto *console = holder()) console->visible = true;
}

void edit(CONSOLES::Console &console, const INPUT::KEYS::Event &event) {
  auto &line = console.line;
  if (event.action == INPUT::KEYS::PRIOR) return CONSOLES::page(console, 1);
  if (event.action == INPUT::KEYS::NEXT) return CONSOLES::page(console, -1);
  if (event.control) return;
  if (event.action == INPUT::KEYS::ENTER) return CONSOLES::submit(console);
  if (event.action == INPUT::KEYS::ERASE) {
    if (!line.empty()) line.pop_back();
    return;
  }
  if (event.codepoint >= ' ' && event.codepoint <= '~')
    line.push_back(static_cast<Char>(event.codepoint));
}
}  // namespace

void TERMINAL::FOCUS::route() {
  for (const auto &event : INPUT::KEYS::drain()) {
    if (event.action == INPUT::KEYS::HOTKEY) {
      summon(event.codepoint);
      continue;
    }
    if (OVERLAY::GET::keyboard()) continue;
    auto *owner = holder();
    if (owner && owner->visible)
      edit(*owner, event);
    else if (INPUT::TEXT::wanted())
      INPUT::TEXT::deliver(event);
  }
  auto *owner = holder();
  const Flag keyboard = OVERLAY::GET::keyboard() || (owner && owner->visible);
  INPUT::KEYS::claim(keyboard);
  INPUT::TEXT::claim(keyboard);
  INPUT::POINTER::claim(OVERLAY::GET::pointer());
}

void TERMINAL::FOCUS::reset() { focused = 0; }
