// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

void LAUNCHER::slide(STRING::Hot id, Float x) {
  auto position = GUI::GET::position(document, id);
  position.x = x;
  GUI::set(document, id, position);
}

void LAUNCHER::span(STRING::Hot id, Float y, Float height) {
  auto position = GUI::GET::position(document, id);
  position.y = y;
  GUI::set(document, id, position);
  auto extent = GUI::GET::extent(document, id);
  extent.h = height;
  GUI::set(document, id, extent);
}

void LAUNCHER::refresh() {
  STAGE::refresh();
  TOOLBAR::refresh();
  LISTING::refresh();
  LINEWORK::refresh();
  CARDS::refresh();
  PREVIEW::refresh();
  SETUP::refresh();
  CREW::refresh();
  FOOTER::refresh();
}
