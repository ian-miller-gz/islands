// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/graphics/text/text.hpp>

#include "state.hpp"

auto LAUNCHER::GET::columns(Whole depth) -> Whole {
  const Float size = GUI::GET::style(document, STYLES::LEAF).size;
  const Float advance =
    GFX::TEXT::GET::advance(size > 0 ? size / GFX::TEXT::EM : 1.0f);
  const Float step = static_cast<Float>(depth) * INDENT;
  const Float sheet = GUI::GET::extent(document, IDS::SHEET).w -
                      GUI::GET::position(document, IDS::SHEET).x;
  const Float width =
    GUI::GET::measured(document, IDS::TREE).w - NAME * 2 - step - sheet;
  if (advance <= 0 || width <= 0) return 0;
  return static_cast<Whole>(width / advance);
}

auto LAUNCHER::GET::picked() -> Whole {
  if (chosen.empty()) return ISLANDS::SELECT::NONE;
  for (Whole at = 0; at < entries.size(); at += 1)
    if (entries[at].directory == chosen) return at;
  return ISLANDS::SELECT::NONE;
}

auto LAUNCHER::GET::noticing() -> Flag {
  return GUI::GET::visibility(document, IDS::NOTICE);
}

auto LAUNCHER::GET::setting() -> Flag {
  return GUI::GET::visibility(document, IDS::SETUP);
}

auto LAUNCHER::GET::plain(const String &text) -> String {
  String run;
  Flag folded = false;
  for (const Char letter : text) {
    const Flag wide = static_cast<unsigned char>(letter) >= 0x80;
    if (wide && folded) continue;
    run += wide ? '-' : letter;
    folded = wide;
  }
  return run;
}

auto LAUNCHER::GET::shortened(const String &text, Whole columns) -> String {
  constexpr STRING::Hot MARK = "...";
  constexpr Whole CUT = 3;
  const String run = plain(text);
  if (!columns || run.size() <= columns) return run;
  if (columns <= CUT) return run.substr(0, columns);
  return run.substr(0, columns - CUT) + MARK;
}
