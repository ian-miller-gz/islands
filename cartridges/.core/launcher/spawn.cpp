// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/relations.hpp>

#include "state.hpp"

auto LAUNCHER::launch(const Entry &entry, pid_t leader) -> pid_t {
  const auto child = RELATIONS::spawn(ISLAND_NAME, entry.directory);
  if (!child) return 0;
  children.push_back(
    {.pid = static_cast<pid_t>(child),
     .name = entry.name,
     .directory = entry.directory,
     .leader = leader});
  if (GET::watched()) WATCH::announce(children.back());
  return children.back().pid;
}

static auto leading(pid_t leader) -> Flag {
  for (const auto &child : LAUNCHER::children)
    if (child.pid == leader) return child.running;
  return false;
}

void LAUNCHER::reap() {
  for (auto &child : children)
    if (child.running && !RELATIONS::GET::alive(static_cast<Whole>(child.pid)))
      child.running = false;
  for (auto &child : children)
    if (child.running && child.leader != 0 && !::leading(child.leader)) {
      stop(child.pid);
      child.leader = 0;
    }
}

void LAUNCHER::stop(pid_t pid) {
  for (const auto &child : children)
    if (child.pid == pid && child.running)
      RELATIONS::kill(static_cast<Whole>(pid));
}

void LAUNCHER::stop() {
  for (const auto &child : children)
    RELATIONS::kill(static_cast<Whole>(child.pid));
  children.clear();
}

void LAUNCHER::dismiss(pid_t pid) {
  WATCH::release(pid);
  for (Whole at = 0; at < children.size(); ++at)
    if (children[at].pid == pid) {
      RELATIONS::kill(static_cast<Whole>(pid));
      children.erase(children.begin() + at);
      return;
    }
}
