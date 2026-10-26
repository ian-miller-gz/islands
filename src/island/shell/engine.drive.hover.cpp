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
#include <generated/profile.hpp>
#include <generated/values.hpp>

#if !defined(SR_PROFILE)
#error "SR_PROFILE is not defined; set it in configs/make.yaml (tokens:)."
#endif

#if SR_PROFILE == SR_DEBUG

namespace {

constexpr Whole POINT = 3;

}  // namespace

void ISLAND::SHELL::hover(::SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("usage: hover <id> [<x> <y>]");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  const Flag stated = session.arguments.size() > POINT;
  const Float x = stated ? STRING::number<Float>(session.arguments[2]) : 0.0f;
  const Float y = stated ? STRING::number<Float>(session.arguments[3]) : 0.0f;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (GUI::hover(document, id.c_str(), x, y) != 0)
    return session.print("hover refused: " + id);
  session.print("hover " + id + " events=" + tail(document, from));
}

#endif
