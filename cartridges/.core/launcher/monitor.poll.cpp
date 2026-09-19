// SPDX-License-Identifier: AGPL-3.0-or-later
#include "monitor/monitor.hpp"

#include "state.hpp"

static const String BEGIN = "begin\n";
static const String END = "end\n";
static const String STATE = "state ";
static constexpr Float PERIOD = 1.0f;

static void latch(pid_t pid) {
  for (auto &child : LAUNCHER::children)
    if (child.pid == pid) child.running = false;
}

static void ingest(const String &block) {
  Whole count = 0;
  for (Whole from = 0; from < block.size();) {
    const auto newline = block.find('\n', from);
    const String line = block.substr(from, newline - from);
    from = newline == String::npos ? block.size() : newline + 1;
    if (!line.starts_with(STATE)) continue;
    const String rest = line.substr(STATE.size());
    const auto first = rest.find(' ');
    const auto second = rest.find(' ', first + 1);
    if (first == String::npos || second == String::npos) continue;
    ++count;
    if (rest.substr(first + 1, second - first - 1) == "stopped")
      latch(static_cast<pid_t>(STRING::number<Integer>(rest.substr(0, first))));
  }
  LAUNCHER::WATCH::tracked = count;
}

static void drain() {
  NETWORK::SESSIONS::receive(LAUNCHER::WATCH::session, LAUNCHER::WATCH::buffer);
  auto &buffer = LAUNCHER::WATCH::buffer;
  const auto end = buffer.rfind(END);
  if (end == String::npos) return;
  const auto begin = buffer.rfind(BEGIN, end);
  if (begin != String::npos) ingest(buffer.substr(begin, end - begin));
  buffer.erase(0, end + END.size());
}

void LAUNCHER::WATCH::attach() {
  session = NETWORK::connect(MONITOR::NAME).handle;
  ready = session != NETWORK::NONE;
}

void LAUNCHER::WATCH::detach() {
  if (session != NETWORK::NONE) NETWORK::SESSIONS::destroy(session);
  session = NETWORK::NONE;
  ready = false;
}

void LAUNCHER::WATCH::poll() {
  if (!ready) return;
  drain();
  const Float now = CLOCK::GET::elapsed();
  if (now - asked < PERIOD) return;
  asked = now;
  if (!NETWORK::SESSIONS::push(session, "poll\n")) detach();
}

void LAUNCHER::WATCH::announce(const LAUNCHER::Child &child) {
  if (!ready) return;
  NETWORK::SESSIONS::push(
    session, "watch " + std::to_string(child.pid) + " " + child.name + "\n");
}

void LAUNCHER::WATCH::release(pid_t pid) {
  if (!ready) return;
  NETWORK::SESSIONS::push(session, "forget " + std::to_string(pid) + "\n");
}
