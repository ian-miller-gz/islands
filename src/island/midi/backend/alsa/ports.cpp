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
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
static const String category = "~/island/midi::";

namespace {
constexpr Integer SHUT = -1;
Vector<Integer> descriptors;
Flag opened = false;

auto claim(const String &node) -> Integer {
  const String path = MIDI::BACKEND::home + "/" + node;
  const Integer descriptor = ::open(path.c_str(), O_RDONLY | O_NONBLOCK);
  if (descriptor == SHUT)
    LOGGER::get(category + "listening")
      .error("MIDI input refused: %s", path.c_str());
  return descriptor;
}
}  // namespace

String MIDI::BACKEND::home = "/dev/snd";
String MIDI::BACKEND::cards = "/proc/asound";

auto MIDI::BACKEND::nodes() -> Vector<String> {
  Vector<String> found;
  DIR *directory = opendir(home.c_str());
  if (!directory) return found;
  while (const dirent *entry = readdir(directory))
    if (String(entry->d_name).rfind(NODE, 0) == 0)
      found.push_back(entry->d_name);
  closedir(directory);
  std::sort(found.begin(), found.end());
  return found;
}

auto MIDI::BACKEND::listening() -> Whole {
  if (opened) return ports();
  opened = true;
  for (const auto &node : nodes()) {
    const Integer descriptor = claim(node);
    if (descriptor != SHUT) descriptors.push_back(descriptor);
  }
  LOGGER::get(category + "listening")
    .info("MIDI inputs opened: %zu", descriptors.size());
  return ports();
}

auto MIDI::BACKEND::ports() -> Whole {
  return static_cast<Whole>(descriptors.size());
}

auto MIDI::BACKEND::take(Whole port, Byte *into, Whole size) -> Whole {
  if (port >= ports() || !size) return 0;
  const ssize_t took =
    ::read(descriptors[port], into, static_cast<size_t>(size));
  if (took <= 0) return 0;
  return static_cast<Whole>(took);
}

void MIDI::BACKEND::close() {
  for (const Integer descriptor : descriptors) ::close(descriptor);
  descriptors.clear();
  opened = false;
}

void MIDI::BACKEND::rehome(const String &path) { home = path; }
