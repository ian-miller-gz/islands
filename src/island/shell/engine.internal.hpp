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
#include <shell/engine/engine.hpp>

namespace ISLAND::SHELL {

void version(::SHELL::Session &session);

void status(::SHELL::Session &session);

auto commands() -> const Vector<::SHELL::Command> &;

auto debug() -> const Vector<::SHELL::Command> *;
void dialog(::SHELL::Session &session);

auto settings() -> const Vector<::SHELL::ENGINE::Setting> &;

namespace LOGS {
auto level() -> String;
auto set(const String &value) -> Flag;
}  // namespace LOGS

}  // namespace ISLAND::SHELL
