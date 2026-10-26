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
#include <island/shell/engine.drive.internal.hpp>
#include <format>
#include <generated/profile.hpp>
#include <generated/values.hpp>

#if !defined(SR_PROFILE)
#error "SR_PROFILE is not defined; set it in configs/make.yaml (tokens:)."
#endif

#if SR_PROFILE == SR_DEBUG

namespace ISLAND::SHELL {
namespace {

auto joined(const Vector<String> &arguments) -> String {
  String text;
  for (Whole index = 1; index < arguments.size(); index += 1) {
    if (!text.empty()) text += ' ';
    text += arguments[index];
  }
  return text;
}

void fed(GUI::Handle document, const String &text) {
  for (const Char letter : text)
    ISLAND::SHELL::fed(
      document, INPUT::KEYS::Event{
                  INPUT::KEYS::TEXT, static_cast<unsigned char>(letter)});
}

}  // namespace

auto composing() -> Flag {
  return INPUT::TEXT::wanted() && GUI::GET::focused() == GUI::NONE;
}

void fed(GUI::Handle document, const INPUT::KEYS::Event &event) {
  if (composing())
    INPUT::TEXT::deliver(event);
  else
    GUI::key(document, event);
}

void type(::SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("usage: type <text>");
  const auto text = joined(session.arguments);
  if (composing()) {
    fed(GUI::NONE, text);
    return session.print(std::format("type{} count={}", LANE, text.size()));
  }
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  fed(document, text);
  session.print("type events=" + tail(document, from));
}

}  // namespace ISLAND::SHELL

#endif
