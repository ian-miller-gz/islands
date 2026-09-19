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
#include <island/midi/backend/ports.hpp>
#include <island/midi/backend/alsa/ports.internal.hpp>
#include <common.hpp>
#include <logger.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

static const String category = "~/island/midi::";

namespace {

constexpr Integer SHUT = -1;
Vector<Integer> outlets;

auto spelled(const String &node) -> String {
  const String stem(MIDI::BACKEND::NODE);
  const auto d = node.find('D', stem.size());
  if (node.rfind(stem, 0) != 0 || d == String::npos) return node;
  const String card = node.substr(stem.size(), d - stem.size());
  const String device = node.substr(d + 1);
  std::ifstream file(MIDI::BACKEND::cards + "/card" + card + "/midi" + device);
  String name;
  if (!std::getline(file, name) || name.empty()) return node;
  return name;
}

}  // namespace

auto MIDI::BACKEND::devices() -> Vector<Device> {
  Vector<Device> found;
  for (const String &node : nodes()) found.push_back({::spelled(node), node});
  return found;
}

auto MIDI::BACKEND::outlet(const String &node, Whole &outlet) -> Flag {
  const String path = home + "/" + node;
  const Integer descriptor = ::open(path.c_str(), O_WRONLY | O_NONBLOCK);
  if (descriptor == SHUT) {
    LOGGER::get(category + "outlet")
      .error("MIDI output refused: %s", path.c_str());
    return false;
  }
  ::outlets.push_back(descriptor);
  outlet = ::outlets.size() - 1;
  return true;
}

auto MIDI::BACKEND::put(Whole outlet, const Byte *from, Whole size) -> Whole {
  if (outlet >= ::outlets.size() || ::outlets[outlet] == SHUT || !size)
    return 0;
  const ssize_t took =
    ::write(::outlets[outlet], from, static_cast<size_t>(size));
  return took <= 0 ? 0 : static_cast<Whole>(took);
}

void MIDI::BACKEND::shut(Whole outlet) {
  if (outlet >= ::outlets.size() || ::outlets[outlet] == SHUT) return;
  ::close(::outlets[outlet]);
  ::outlets[outlet] = SHUT;
}

void MIDI::BACKEND::shut() {
  for (Integer &descriptor : ::outlets)
    if (descriptor != SHUT) ::close(descriptor);
  ::outlets.clear();
}
