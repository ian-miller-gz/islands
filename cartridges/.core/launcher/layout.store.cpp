// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

namespace LAUNCHER::STORE {
namespace {
constexpr STRING::Hot COLUMNS[] = {
  LAUNCHER::IDS::CALLED,  LAUNCHER::IDS::VERSION,  LAUNCHER::IDS::DETAIL,
  LAUNCHER::IDS::INSTALL, LAUNCHER::IDS::STANDING, LAUNCHER::IDS::PROGRESS};

void show(Whole at, Flag standing) {
  for (STRING::Hot column : COLUMNS)
    GUI::set(
      LAUNCHER::document, LAUNCHER::IDS::cell(column, at).c_str(),
      GUI::Visibility{standing});
}

void cell(STRING::Hot column, Whole at, const String &text) {
  GUI::set(
    LAUNCHER::document, LAUNCHER::IDS::cell(column, at).c_str(),
    GUI::Text{text});
}

void dress(STRING::Hot column, Whole at, STRING::Hot style) {
  GUI::set(
    LAUNCHER::document, LAUNCHER::IDS::cell(column, at).c_str(),
    GUI::Style{style});
}

auto face(const LAUNCHER::STORE::Offer &offer) -> STRING::Hot {
  namespace STORE = LAUNCHER::STORE;
  if (offer.installed) return STORE::CROSS;
  return STORE::installing == ISLANDS::SELECT::NONE ? STORE::OFFERED
                                                    : STORE::BUSY;
}

void gauge(Whole at) {
  namespace IDS = LAUNCHER::IDS;
  const Flag filling = LAUNCHER::STORE::installing == at;
  GUI::set(
    LAUNCHER::document, IDS::cell(IDS::PROGRESS, at).c_str(),
    GUI::Visibility{filling});
  if (filling)
    GUI::set(
      LAUNCHER::document, IDS::cell(IDS::PROGRESS, at).c_str(),
      GUI::Fraction{LAUNCHER::STORE::GET::progress()});
}

void write(Whole at, const LAUNCHER::STORE::Offer &offer) {
  namespace IDS = LAUNCHER::IDS;
  namespace STORE = LAUNCHER::STORE;
  namespace STYLES = LAUNCHER::STYLES;
  cell(IDS::CALLED, at, LAUNCHER::GET::plain(offer.name));
  cell(IDS::VERSION, at, LAUNCHER::GET::plain(offer.release));
  cell(IDS::DETAIL, at, LAUNCHER::GET::plain(offer.description));
  cell(IDS::STANDING, at, offer.installed ? String(STORE::MARK) : offer.state);
  dress(IDS::STANDING, at, offer.installed ? STYLES::MARK : STYLES::MUTED);
  cell(IDS::INSTALL, at, face(offer));
  dress(IDS::INSTALL, at, offer.installed ? STYLES::DROP : STYLES::PLACE);
  gauge(at);
}
}  // namespace
}  // namespace LAUNCHER::STORE

void LAUNCHER::STORE::refresh() {
  if (showing != IDS::OFFERS) return;
  for (Whole at = 0; at < ROWS; at += 1) {
    show(at, at < offers.size());
    if (at >= offers.size()) continue;
    installed(offers[at]);
    write(at, offers[at]);
  }
}
