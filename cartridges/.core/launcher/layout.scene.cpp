// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/requirements.hpp>

#include "state.hpp"

static auto facts(const LAUNCHER::Entry &entry) -> Vector<String> {
  Vector<String> lines;
  if (!entry.author.empty())
    lines.push_back("by " + LAUNCHER::GET::plain(entry.author));
  if (!entry.release.empty())
    lines.push_back("release " + LAUNCHER::GET::plain(entry.release));
  if (!entry.updated.empty()) lines.push_back("updated " + entry.updated);
  return lines;
}

static auto packed(const Vector<String> &lines) -> Float {
  constexpr Float GAP = 12;
  const String head = LAUNCHER::IDS::fact(0), next = LAUNCHER::IDS::fact(1);
  const Float top = GUI::GET::position(LAUNCHER::document, head.c_str()).y;
  const Float step =
    GUI::GET::position(LAUNCHER::document, next.c_str()).y - top;
  for (Whole at = 0; at < LAUNCHER::IDS::FACTS; at += 1) {
    const String id = LAUNCHER::IDS::fact(at);
    GUI::set(
      LAUNCHER::document, id.c_str(),
      GUI::Text{at < lines.size() ? lines[at] : String()});
  }
  return top + step * static_cast<Float>(lines.size()) + GAP;
}

void LAUNCHER::CARDS::refresh() {
  const Float top = GUI::GET::position(document, IDS::SHEET).y;
  const Float below = GUI::GET::extent(document, IDS::WATCH).h -
                      GUI::GET::position(document, IDS::WATCH).y;
  const Float height =
    GUI::GET::measured(document, IDS::BROWSER).h - top * 2 - below;
  if (height > 0) span(IDS::SHEET, top, height);
}

void LAUNCHER::PREVIEW::refresh() {
  const Whole at = GET::picked();
  const Flag picked = at < entries.size();
  const Entry entry = picked ? entries[at] : Entry{};
  const Vector<String> lines = picked ? ::facts(entry) : Vector<String>{};
  GUI::set(
    document, IDS::NAMED,
    GUI::Text{picked ? GET::plain(entry.name) : String()});
  span(IDS::SUMMARY, ::packed(lines), 0);
  GUI::set(
    document, IDS::SUMMARY,
    GUI::Text{
      GET::plain(picked && !entry.built ? "Not built yet." : entry.summary)});
}

void LAUNCHER::CREW::refresh() {
  GUI::set(document, IDS::QUIET, GUI::Visibility{children.empty()});
  GUI::set(document, IDS::CREW, GUI::Rows{children.size()});
  const Whole first = GUI::GET::first(document, IDS::CREW);
  const Whole window = GUI::GET::window(document, IDS::CREW);
  for (Whole at = 0; at < window; at += 1) {
    const Whole index = first + at;
    if (index >= children.size()) break;
    const Child &child = children[index];
    const String mark = child.running ? "running" : "stopped";
    GUI::set(
      document, IDS::item(IDS::CREW, at).c_str(),
      GUI::Text{GET::plain(mark + "  " + child.name)});
  }
}

void LAUNCHER::STAGE::refresh() {
  for (const Scene &scene : SCENES) {
    const Flag standing = showing == scene.panel;
    GUI::set(document, scene.panel, GUI::Visibility{standing});
    GUI::set(
      document, scene.call,
      GUI::Style{standing ? STYLES::SELECTED : STYLES::PLACE});
  }
}

void LAUNCHER::TOOLBAR::refresh() {
  for (const Area &area : AREAS)
    GUI::set(
      document, area.id,
      GUI::Style{
        location == area.directory ? STYLES::SELECTED : STYLES::PLACE});
}

void LAUNCHER::FOOTER::refresh() {
  const Whole at = GET::picked();
  const Flag runnable = at < entries.size() && entries[at].built;
  GUI::set(
    document, IDS::NOTE,
    GUI::Text{GET::plain(
      REQUIREMENTS::GET::met(WATCH::BUNDLE) ? "Attached to monitor reef."
                                            : "Monitor Unavailable")});
  GUI::set(
    document, IDS::LAUNCH, GUI::Style{runnable ? STYLES::CALL : STYLES::STILL});
}
