// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

auto LAUNCHER::GET::worn(const Entry &entry) -> String {
  constexpr STRING::Hot UNBUILT = "  (not built)";
  if (!entry.bundle) return entry.name;
  return entry.built ? entry.name : entry.name + UNBUILT;
}

auto LAUNCHER::GET::folded(const Entry &entry) -> GUI::Fold {
  if (entry.bundle) return {GUI::Fold::NONE};
  return {entry.open ? GUI::Fold::OPEN : GUI::Fold::CLOSED};
}

void LAUNCHER::LISTING::refresh() {
  GUI::set(document, IDS::BARE, GUI::Visibility{entries.empty()});
  GUI::set(document, IDS::TREE, GUI::Rows{entries.size()});
  const Whole first = GUI::GET::first(document, IDS::TREE);
  const Whole window = GUI::GET::window(document, IDS::TREE);
  for (Whole at = 0; at < window; at += 1) {
    const Whole index = first + at;
    if (index >= entries.size()) break;
    const String id = IDS::item(IDS::TREE, at);
    const Entry &entry = entries[index];
    const Float indent = static_cast<Float>(entry.depth) * INDENT;
    GUI::set(document, id.c_str(), GUI::Pad{NAME + indent});
    GUI::set(document, id.c_str(), GET::folded(entry));
    GUI::set(
      document, id.c_str(),
      GUI::Text{GET::shortened(GET::worn(entry), GET::columns(entry.depth))});
  }
}
