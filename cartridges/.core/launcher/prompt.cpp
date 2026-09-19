// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/dialog.hpp>

#include "state.hpp"

auto LAUNCHER::prompt(GFX::Viewport viewport) -> Flag {
  if (!GUI::DIALOG::GET::active()) return false;
  GUI::DIALOG::place(viewport);
  GUI::DIALOG::poll();
  return true;
}
