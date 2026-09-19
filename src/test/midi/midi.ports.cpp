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
#include <island/midi/backend/selection.hpp>
#include <iostream>
#include "midi.internal.hpp"

#if SR_MIDI_BACKEND == SR_ALSA
#include <island/midi/backend/alsa/ports.internal.hpp>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {
constexpr mode_t GRANT = S_IRUSR | S_IWUSR;
constexpr Whole SEATS = 4;

auto sent(Integer port, const Vector<Whole> &tape, Vector<MIDI::Message> &into)
  -> Whole {
  Vector<Byte> raw;
  for (const Whole byte : tape) raw.push_back(static_cast<Byte>(byte));
  if (write(port, raw.data(), raw.size()) != static_cast<ssize_t>(raw.size()))
    return 0;
  return MIDI::read(into);
}

auto matches(
  const MIDI::Message &message, Whole kind, Whole channel, Whole first,
  Whole second) -> Flag {
  return message.kind == kind && message.channel == channel &&
         message.first == first && message.second == second;
}

auto nest(String &home, String &first, String &other) -> Flag {
  char made[] = "/tmp/islands-midi-XXXXXX";
  if (!mkdtemp(made)) return false;
  home = made;
  first = home + "/" + MIDI::BACKEND::NODE + "0D0";
  other = home + "/" + MIDI::BACKEND::NODE + "1D0";
  return mkfifo(first.c_str(), GRANT) == 0 && mkfifo(other.c_str(), GRANT) == 0;
}
}  // namespace

auto heard() -> Flag {
  String home, first, other;
  if (!nest(home, first, other)) return false;
  MIDI::BACKEND::home = home;
  Vector<MIDI::Message> into(SEATS);
  Flag ok = MIDI::read(into) == 0 && MIDI::GET::ports() == 2;
  const Integer near = open(first.c_str(), O_WRONLY | O_NONBLOCK);
  const Integer far = open(other.c_str(), O_WRONLY | O_NONBLOCK);
  ok = ok && near >= 0 && far >= 0;
  ok = ok && sent(near, {0x90, 0x3C, 0x40}, into) == 1 &&
       matches(into[0], MIDI::Message::NOTE_ON, 0, 0x3C, 0x40);
  ok = ok && sent(far, {0xB1, 0x07, 0x64}, into) == 1 &&
       matches(into[0], MIDI::Message::CONTROL, 1, 0x07, 0x64);
  Vector<MIDI::Message> narrow(1);
  ok = ok && sent(near, {0x90, 0x3C, 0x40, 0x3E, 0x50}, narrow) == 1 &&
       MIDI::read(narrow) == 1 &&
       matches(narrow[0], MIDI::Message::NOTE_ON, 0, 0x3E, 0x50);
  close(near);
  close(far);
  MIDI::close();
  ok = ok && MIDI::GET::ports() == 0;
  unlink(first.c_str());
  unlink(other.c_str());
  rmdir(home.c_str());
  return ok && MIDI::read(into) == 0 && MIDI::GET::ports() == 0;
}
auto spoken() -> Flag {
  String home, first, other;
  if (!nest(home, first, other)) return false;
  MIDI::BACKEND::home = home;
  MIDI::close();
  const Vector<MIDI::Device> roster = MIDI::GET::devices();
  Flag ok = roster.size() == 2 && roster[0].name == roster[0].node &&
            roster[0].node == String(MIDI::BACKEND::NODE) + "0D0";
  Vector<MIDI::Message> into(SEATS);
  ok = ok && MIDI::read(into) == 0 && MIDI::GET::ports() == 2;
  const MIDI::Handle out = MIDI::OUTPUT::create(roster[0].name);
  ok = ok && out != MIDI::OUTPUT::NONE;
  ok = ok && MIDI::OUTPUT::create("nowhere") == MIDI::OUTPUT::NONE;
  MIDI::OUTPUT::send(out, {MIDI::Message::NOTE_ON, 2, 0x3C, 0x40});
  MIDI::OUTPUT::send(out, {MIDI::Message::PROGRAM, 2, 0x05, 0});
  ok = ok && MIDI::read(into) == 2 &&
       matches(into[0], MIDI::Message::NOTE_ON, 2, 0x3C, 0x40) &&
       matches(into[1], MIDI::Message::PROGRAM, 2, 0x05, 0);
  MIDI::OUTPUT::remove(out);
  MIDI::OUTPUT::send(out, {MIDI::Message::NOTE_OFF, 2, 0x3C, 0});
  ok = ok && MIDI::read(into) == 0;
  MIDI::close();
  ok = ok && MIDI::GET::ports() == 0;
  unlink(first.c_str());
  unlink(other.c_str());
  rmdir(home.c_str());
  return ok && MIDI::GET::devices().empty();
}
#else
auto heard() -> Flag {
  std::cout << "ports: unsupported (SR_MIDI_BACKEND: SR_NONE)" << std::endl;
  Vector<MIDI::Message> into(1);
  return MIDI::read(into) == 0 && MIDI::GET::ports() == 0;
}
auto spoken() -> Flag {
  return MIDI::GET::devices().empty() &&
         MIDI::OUTPUT::create("nowhere") == MIDI::OUTPUT::NONE;
}
#endif
