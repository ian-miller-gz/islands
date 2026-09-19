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

#include <network/instances/selection.hpp>
#include <topics/selection.hpp>

namespace SHELL::ENGINE::ACTIONS {
void echo(Session &session);
void help(Session &session);
void metrics(Session &session);
void configure(Session &session);
void status(Session &session);
void log(Session &session);
void web(Session &session);
#if SR_LISTEN == SR_UNIX
void instances(Session &session);
#endif
#if SR_TOPICS == SR_LOCAL
void topics(Session &session);
#endif
}  // namespace SHELL::ENGINE::ACTIONS
