// SPDX-License-Identifier: AGPL-3.0-or-later
#include <signal.h>

#include "monitor.internal.hpp"

static auto found(pid_t pid) -> MONITOR::Entry * {
  for (auto &entry : MONITOR::roster)
    if (entry.pid == pid) return &entry;
  return nullptr;
}

static void watch(const String &rest) {
  const auto space = rest.find(' ');
  if (space == String::npos) return;
  const auto pid =
    static_cast<pid_t>(STRING::number<Integer>(rest.substr(0, space)));
  const String bundle = rest.substr(space + 1);
  if (auto *entry = found(pid)) {
    entry->bundle = bundle;
    entry->running = true;
    return;
  }
  MONITOR::roster.push_back({pid, bundle, true});
}

static void forget(const String &rest) {
  const auto pid = static_cast<pid_t>(STRING::number<Integer>(rest));
  for (Whole at = 0; at < MONITOR::roster.size(); ++at)
    if (MONITOR::roster[at].pid == pid) {
      MONITOR::roster.erase(MONITOR::roster.begin() + at);
      return;
    }
}

void MONITOR::dispatch(const String &line) {
  const auto space = line.find(' ');
  const String verb = line.substr(0, space);
  if (space == String::npos) return;
  const String rest = line.substr(space + 1);
  if (verb == "watch") return watch(rest);
  if (verb == "forget") return forget(rest);
}

void MONITOR::sweep() {
  for (auto &entry : roster) entry.running = kill(entry.pid, 0) == 0;
}

auto MONITOR::snapshot() -> String {
  String block = "begin\n";
  for (const auto &entry : roster)
    block += "state " + std::to_string(entry.pid) + " " +
             (entry.running ? "running" : "stopped") + " " + entry.bundle +
             "\n";
  return block + "end\n";
}
