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
#include <island/shell/engine.internal.hpp>
#include <generated/profile.hpp>
#include <generated/values.hpp>
#include <charconv>

#if !defined(SR_PROFILE)
#error "SR_PROFILE is not defined; set it in configs/make.yaml (tokens:)."
#endif

#if SR_PROFILE == SR_DEBUG

namespace ISLAND::SHELL::DRIVE {
namespace {

struct Stroke {
  STRING::Hot name;
  INPUT::KEYS::Action action;
  Whole codepoint = 0;
};

constexpr Whole CASED = 'a' - 'A';
constexpr Whole LETTER = 1;
constexpr char FUNCTION = 'f';
constexpr Whole FUNCTIONS = 12;

constexpr Stroke STROKES[] = {
  {"enter", INPUT::KEYS::ENTER},
  {"erase", INPUT::KEYS::ERASE},
  {"delete", INPUT::KEYS::DELETE},
  {"left", INPUT::KEYS::LEFT},
  {"right", INPUT::KEYS::RIGHT},
  {"up", INPUT::KEYS::UP},
  {"down", INPUT::KEYS::DOWN},
  {"home", INPUT::KEYS::HOME},
  {"end", INPUT::KEYS::END},
  {"prior", INPUT::KEYS::PRIOR},
  {"next", INPUT::KEYS::NEXT},
  {"tab", INPUT::KEYS::TEXT, INPUT::KEYS::TAB},
  {"escape", INPUT::KEYS::TEXT, INPUT::KEYS::ESCAPE}};

auto functioned(const String &name, INPUT::KEYS::Event &event) -> Flag {
  if (name.size() <= LETTER || name[0] != FUNCTION) return false;
  const char *end = name.data() + name.size();
  Whole ordinal = 0;
  const auto [stop, fault] =
    std::from_chars(name.data() + LETTER, end, ordinal);
  if (fault != std::errc{} || stop != end) return false;
  if (ordinal == 0 || ordinal > FUNCTIONS) return false;
  event = {INPUT::KEYS::HOTKEY, ordinal};
  return true;
}

}  // namespace

auto composed(const String &name, INPUT::KEYS::Event &event) -> Flag {
  for (const auto &entry : STROKES) {
    if (name != entry.name) continue;
    event = {entry.action, entry.codepoint};
    return true;
  }
  if (functioned(name, event)) return true;
  if (name.size() != LETTER) return false;
  const Whole letter = static_cast<unsigned char>(name[0]);
  event = {
    INPUT::KEYS::TEXT,
    letter >= 'a' && letter <= 'z' ? letter - CASED : letter};
  return true;
}

}  // namespace ISLAND::SHELL::DRIVE

#endif
