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
#include <island/gui/dialog.internal.hpp>
#include <generated/profile.hpp>
#include <generated/values.hpp>

#if !defined(SR_PROFILE)
#error "SR_PROFILE is not defined; set it in configs/make.yaml (tokens:)."
#endif

#if SR_PROFILE == SR_DEBUG

namespace {

constexpr GFX::Viewport VIEWPORT = {0.0f, 0.0f, 640.0f, 480.0f};

}  // namespace

void ISLAND::SHELL::dialog(::SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("usage: dialog <path> [filter]");
  const auto &path = session.arguments[1];
  const auto filter =
    session.arguments.size() > 2 ? session.arguments[2] : String();
  GUI::DIALOG::raise(GUI::DIALOG::OPEN, path.c_str(), filter.c_str());
  GUI::DIALOG::place(VIEWPORT);
  GUI::DIALOG::poll();
  GFX::Pass pass = {.viewport = VIEWPORT};
  GUI::DIALOG::flush(pass);
  session.print("dialog " + path + (filter.empty() ? "" : " " + filter));
}

#endif
