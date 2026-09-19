#pragma region COPYRIGHT
/* ===========================================================================
 *              (C) Copyright 2025 - Islands-Engine - Ian Miller             *
 =============================================================================
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Affero General Public License as           *
 *  published by the Free Software Foundation, either version 3 of the       *
 *  License, or (at your option) any later version.                          *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Affero General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Affero General Public License *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ============================================================================*/
#pragma endregion
#pragma once

#include <common.hpp>
#include <network/listeners.hpp>
#include <network/lines.hpp>
#include <network/sessions.hpp>

namespace NETWORK::PEERS {

struct Bare {};

template <typename State = Bare>
struct Peer {
  Handle session = NONE;
  LINES::Framer lines;
  State state{};

  auto receive() -> Flag;
  auto next(String &line) -> Flag;
  void close();
};

template <typename State = Bare>
class Peers {
 public:
  void accept(Handle listener);
  void reap();

  auto begin() { return roll.begin(); }
  auto end() { return roll.end(); }
  auto size() const -> Whole { return roll.size(); }

 private:
  Vector<Peer<State>> roll;
};

template <typename State>
auto Peer<State>::receive() -> Flag {
  if (session == NONE) return false;
  String bytes;
  Flag alive = SESSIONS::receive(session, bytes);
  lines.feed(bytes);
  return alive;
}

template <typename State>
auto Peer<State>::next(String &line) -> Flag {
  return lines.next(line);
}

template <typename State>
void Peer<State>::close() {
  if (session == NONE) return;
  SESSIONS::destroy(session);
  session = NONE;
}

template <typename State>
void Peers<State>::accept(Handle listener) {
  for (auto session = LISTENERS::accept(listener); session != NONE;
       session = LISTENERS::accept(listener))
    roll.push_back(Peer<State>{.session = session});
}

template <typename State>
void Peers<State>::reap() {
  for (Whole at = 0; at < roll.size();)
    if (roll[at].session == NONE)
      roll.erase(roll.begin() + at);
    else
      ++at;
}

}  // namespace NETWORK::PEERS
