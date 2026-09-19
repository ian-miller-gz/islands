// SPDX-License-Identifier: AGPL-3.0-or-later
#include <fstream>

#include "state.hpp"

auto LAUNCHER::GET::bare(const String &line) -> String {
  const auto first = line.find_first_not_of(" \t");
  if (first == String::npos) return {};
  const String text = line.substr(first);
  return text.starts_with("- ") ? text.substr(2) : text;
}

static void dress(
  LAUNCHER::Control &control, const String &key, const String &value) {
  if (key == "label")
    control.label = value;
  else if (key == "summary")
    control.summary = value;
  else if (key == "runs")
    control.runs = value;
}

static void vet() {
  for (LAUNCHER::Control &control : LAUNCHER::controls) {
    if (control.runs.empty()) continue;
    LAUNCHER::Entry sub{.directory = LAUNCHER::chosen + "/" + control.runs};
    LAUNCHER::identify(sub);
    control.ready = sub.built;
  }
}

constexpr STRING::Hot CORE = "cartridges/.core/";

static void offer() {
  if (!LAUNCHER::chosen.starts_with(CORE)) return;
  if (LAUNCHER::controls.size() >= LAUNCHER::SETUP::BOXES) return;
  LAUNCHER::controls.push_back(
    {.id = "watch",
     .label = "Monitor",
     .summary = "enable to attach monitor",
     .on = true,
     .ready = true});
}

void LAUNCHER::SETUP::read() {
  controls.clear();
  if (chosen.empty()) return;
  std::ifstream in(chosen + "/settings.yaml");
  String line, key, value;
  while (std::getline(in, line)) {
    const String text = GET::bare(line);
    if (text.empty() || text[0] == '#') continue;
    if (!STRING::pair(text, key, value)) continue;
    if (key == "check" && controls.size() < BOXES) {
      controls.push_back({.id = value, .label = value});
      continue;
    }
    if (!controls.empty()) ::dress(controls.back(), key, value);
  }
  ::vet();
  ::offer();
  for (Whole box = 0; box < BOXES; box += 1)
    GUI::set(document, IDS::note(box).c_str(), GUI::Opening{false});
}

void LAUNCHER::tick(Whole box) {
  if (box >= controls.size() || !controls[box].ready) return;
  controls[box].on = !controls[box].on;
}

auto LAUNCHER::GET::watched() -> Flag {
  for (const Control &control : controls)
    if (control.runs.empty() && control.id == "watch") return control.on;
  return true;
}
