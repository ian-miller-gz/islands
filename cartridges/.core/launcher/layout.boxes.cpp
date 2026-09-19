// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/graphics/text/text.hpp>

#include "state.hpp"

static auto columns(const String &label, Float until) -> Whole {
  const Float size = GUI::GET::size(LAUNCHER::document, label.c_str());
  const Float advance =
    GFX::TEXT::GET::advance(size > 0 ? size / GFX::TEXT::EM : 1.0f);
  const Float from = GUI::GET::position(LAUNCHER::document, label.c_str()).x;
  if (advance <= 0 || until <= from) return 0;
  return static_cast<Whole>((until - from) / advance);
}

static void coat(const String &note) {
  const auto id = note.c_str();
  GUI::set(
    LAUNCHER::document, id,
    GUI::Style{
      GUI::GET::opening(LAUNCHER::document, id) ? LAUNCHER::STYLES::UNFOLDED
                                                : LAUNCHER::STYLES::FOLDED});
}

static void write(Whole box, const LAUNCHER::Control &control) {
  constexpr STRING::Hot ON = "[x] ", OFF = "[ ] ";
  const String word = LAUNCHER::IDS::word(box);
  const String note = LAUNCHER::IDS::note(box);
  const Float seat = GUI::GET::position(LAUNCHER::document, note.c_str()).x;
  GUI::set(
    LAUNCHER::document, word.c_str(),
    GUI::Text{LAUNCHER::GET::shortened(
      String(control.on ? ON : OFF) + control.label, ::columns(word, seat))});
  GUI::set(
    LAUNCHER::document, note.c_str(),
    GUI::Text{control.ready ? control.summary : "not built"});
  GUI::set(
    LAUNCHER::document, word.c_str(),
    GUI::Style{
      control.ready ? LAUNCHER::STYLES::INK : LAUNCHER::STYLES::STILL});
  ::coat(note);
}

void LAUNCHER::SETUP::refresh() {
  if (!GET::setting()) return;
  const Whole at = GET::picked();
  GUI::set(
    document, IDS::TITLED,
    GUI::Text{
      (at < entries.size() ? GET::plain(entries[at].name) + " " : String()) +
      "Options"});
  for (Whole box = 0; box < BOXES; box += 1) {
    const String id = IDS::box(box);
    GUI::set(document, id.c_str(), GUI::Visibility{box < controls.size()});
    if (box < controls.size()) ::write(box, controls[box]);
  }
}
