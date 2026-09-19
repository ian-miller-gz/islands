// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

struct Seat {
  Float top = 0;
  Float height = 0;
};

static auto seat(Whole row) -> Seat {
  const auto corner =
    GUI::GET::position(LAUNCHER::document, LAUNCHER::IDS::TREE);
  const Float pitch = GUI::GET::pitch(LAUNCHER::document, LAUNCHER::IDS::TREE);
  const Float pad = GUI::GET::pad(LAUNCHER::document, LAUNCHER::IDS::TREE);
  return {corner.y + static_cast<Float>(row) * (pitch + pad), pitch};
}

static auto foot(Whole row, Whole entry, Whole window) -> Float {
  constexpr Float MIDDLE = 2;
  const Whole depth = LAUNCHER::entries[entry].depth;
  const Whole first = GUI::GET::first(LAUNCHER::document, LAUNCHER::IDS::TREE);
  Float end = 0;
  for (Whole at = row + 1; at < window; at += 1) {
    const Whole index = first + at;
    if (index >= LAUNCHER::entries.size()) break;
    if (LAUNCHER::entries[index].depth <= depth) break;
    const Seat below = ::seat(at);
    end = below.top + (LAUNCHER::entries[index].depth == depth + 1
                         ? below.height / MIDDLE
                         : below.height);
  }
  return end;
}

static auto draw(Whole slot, Whole row, Whole entry, Whole window) -> Flag {
  const Seat above = ::seat(row);
  const Float top = above.top + above.height;
  const Float end = ::foot(row, entry, window);
  if (end <= top) return false;
  const String id = LAUNCHER::IDS::rule(slot);
  const Float inset =
    GUI::GET::position(LAUNCHER::document, LAUNCHER::IDS::TREE).x;
  const Float step =
    static_cast<Float>(LAUNCHER::entries[entry].depth) * LAUNCHER::INDENT;
  LAUNCHER::slide(id.c_str(), inset + LAUNCHER::STEM + step);
  LAUNCHER::span(id.c_str(), top, end - top);
  GUI::set(LAUNCHER::document, id.c_str(), GUI::Visibility{true});
  return true;
}

static void elbow(Whole at, Whole index, Whole window) {
  constexpr Float MIDDLE = 2;
  const String id = LAUNCHER::IDS::tick(at);
  const Flag shown = at < window && index < LAUNCHER::entries.size() &&
                     LAUNCHER::entries[index].depth > 0;
  GUI::set(LAUNCHER::document, id.c_str(), GUI::Visibility{shown});
  if (!shown) return;
  const Seat here = ::seat(at);
  const Float inset =
    GUI::GET::position(LAUNCHER::document, LAUNCHER::IDS::TREE).x;
  const Float step =
    static_cast<Float>(LAUNCHER::entries[index].depth) * LAUNCHER::INDENT;
  auto position = GUI::GET::position(LAUNCHER::document, id.c_str());
  position.x = inset + LAUNCHER::STEM + step - LAUNCHER::INDENT;
  position.y = here.top + here.height / MIDDLE;
  GUI::set(LAUNCHER::document, id.c_str(), position);
}

void LAUNCHER::LINEWORK::refresh() {
  const Whole window = GUI::GET::window(document, IDS::TREE);
  const Whole first = GUI::GET::first(document, IDS::TREE);
  Whole used = 0;
  for (Whole at = 0; at < window && used < RULES; at += 1) {
    const Whole index = first + at;
    if (index >= entries.size()) break;
    if (entries[index].open && ::draw(used, at, index, window)) used += 1;
  }
  for (Whole at = used; at < RULES; at += 1) {
    const String id = IDS::rule(at);
    GUI::set(document, id.c_str(), GUI::Visibility{false});
  }
  for (Whole at = 0; at < TICKS; at += 1) ::elbow(at, first + at, window);
}
