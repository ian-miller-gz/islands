// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

static auto subtree(const Vector<LAUNCHER::Entry> &rows, Whole entry) -> Whole {
  const Whole depth = rows[entry].depth;
  Whole end = entry + 1;
  while (end < rows.size() && rows[end].depth > depth) end += 1;
  return end;
}

void LAUNCHER::toggle(Vector<Entry> &rows, Whole entry) {
  Entry &row = rows[entry];
  const Whole end = ::subtree(rows, entry);
  row.open = !row.open;
  if (!row.open) {
    rows.erase(rows.begin() + entry + 1, rows.begin() + end);
    return;
  }
  const Vector<Entry> children = list(row.directory, row.depth + 1);
  rows.insert(rows.begin() + entry + 1, children.begin(), children.end());
}

void LAUNCHER::toggle(Whole entry) { toggle(entries, entry); }

void LAUNCHER::choose(Whole entry) {
  if (entry >= entries.size() || !entries[entry].bundle) return;
  if (chosen == entries[entry].directory) return;
  chosen = entries[entry].directory;
  SETUP::read();
}

void LAUNCHER::open(Whole entry) {
  if (entry >= entries.size()) return;
  if (!entries[entry].bundle) return toggle(entry);
  choose(entry);
}

void LAUNCHER::visit(STRING::Hot area) {
  for (const Area &known : AREAS) {
    if (String(known.id) != area) continue;
    location = known.directory;
    scan();
    return;
  }
}

void LAUNCHER::enter(STRING::Hot call) {
  for (const Scene &scene : SCENES) {
    if (String(scene.call) != call) continue;
    showing = showing == scene.panel ? String() : String(scene.panel);
  }
  if (showing == IDS::OFFERS) STORE::survey();
}

void LAUNCHER::start() {
  const Whole at = GET::picked();
  if (at >= entries.size()) return;
  const Entry &entry = entries[at];
  if (!entry.bundle || !entry.built) return;
  const pid_t leader = launch(entry, 0);
  if (leader == 0) return;
  for (const Control &control : controls)
    if (control.on && control.ready && !control.runs.empty())
      launch(
        {.name = entry.name + "/" + control.id,
         .directory = entry.directory + "/" + control.runs,
         .built = true},
        leader);
}
