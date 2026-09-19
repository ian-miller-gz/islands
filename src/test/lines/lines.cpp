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
#include <iostream>
#include <network.hpp>

using NETWORK::LINES::Framer;

namespace {

auto drain(Framer &framer) -> Vector<String> {
  Vector<String> out;
  for (String line; framer.next(line);) out.push_back(line);
  return out;
}

auto whole_and_fragment() -> Flag {
  Framer framer;
  framer.feed("alpha\nbeta\ngam");
  auto first = drain(framer);
  if (first.size() != 2 || first[0] != "alpha" || first[1] != "beta")
    return false;
  framer.feed("ma\n");
  auto second = drain(framer);
  return second.size() == 1 && second[0] == "gamma";
}

auto carriage_return() -> Flag {
  Framer framer;
  framer.feed("crlf\r\n\nbare\n");
  auto lines = drain(framer);
  return lines.size() == 3 && lines[0] == "crlf" && lines[1].empty() &&
         lines[2] == "bare";
}

auto oversized() -> Flag {
  Framer framer;
  framer.feed(String(NETWORK::LINES::LIMIT + 1, 'x'));
  if (!drain(framer).empty() || framer.dropped() != 1) return false;
  framer.feed("still-the-runaway\nafter\n");
  auto lines = drain(framer);
  return lines.size() == 1 && lines[0] == "after" && framer.dropped() == 1;
}

auto registry() -> Flag {
  NETWORK::PEERS::Peers<> peers;
  peers.accept(NETWORK::NONE);
  peers.reap();
  return peers.size() == 0;
}

struct Case {
  const char *name;
  Flag (*run)();
};

}  // namespace

int main() {
  const Case cases[] = {
    {"whole-and-fragment", whole_and_fragment},
    {"carriage-return", carriage_return},
    {"oversized-drop", oversized},
    {"peers-registry", registry},
  };
  Flag ok = true;
  for (const auto &item : cases) {
    Flag pass = item.run();
    std::cout << "lines " << item.name << ": " << (pass ? "PASS" : "FAIL")
              << std::endl;
    ok = ok && pass;
  }
  return ok ? 0 : 1;
}
