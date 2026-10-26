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
#include <shell/os/keys.hpp>

namespace IN = INPUT::KEYS;

namespace {
struct Code {
  IN::Action action;
  STRING::Hot sequence;
};
constexpr Code CURSORS[] = {
  {IN::LEFT, "\x1bOD"}, {IN::RIGHT, "\x1bOC"}, {IN::UP, "\x1bOA"},
  {IN::DOWN, "\x1bOB"}, {IN::HOME, "\x1bOH"},  {IN::END, "\x1bOF"},
};
constexpr Code CODES[] = {
  {IN::LEFT, "\x1b[D"},    {IN::RIGHT, "\x1b[C"},  {IN::UP, "\x1b[A"},
  {IN::DOWN, "\x1b[B"},    {IN::HOME, "\x1b[H"},   {IN::END, "\x1b[F"},
  {IN::DELETE, "\x1b[3~"}, {IN::PRIOR, "\x1b[5~"}, {IN::NEXT, "\x1b[6~"},
  {IN::ENTER, "\r"},       {IN::ERASE, "\x7f"},
};
}  // namespace

auto SHELL::OS::KEYS::encode(const IN::Event &event) -> String {
  return encode(event, false);
}

auto SHELL::OS::KEYS::encode(const IN::Event &event, Flag cursors) -> String {
  if (cursors)
    for (const auto &code : CURSORS)
      if (code.action == event.action) return code.sequence;
  for (const auto &code : CODES)
    if (code.action == event.action) return code.sequence;
  if (event.action != IN::TEXT) return {};
  if (event.control)
    return String(1, static_cast<Char>(event.codepoint & 0x1f));
  if (event.codepoint == 0 || event.codepoint > 0x7f) return {};
  return String(1, static_cast<Char>(event.codepoint));
}
