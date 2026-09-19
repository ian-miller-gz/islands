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

#include <generated/values.hpp>
#include <generated/topics.hpp>

#if !defined(SR_TOPICS) || !defined(SR_NONE) || !defined(SR_LOCAL)
#error \
  "SR_TOPICS/SR_NONE/SR_LOCAL are not defined; set them in configs/make.yaml (tokens:)."
#elif SR_TOPICS != SR_NONE && SR_TOPICS != SR_LOCAL
#error "SR_TOPICS has an unknown value; expected SR_NONE or SR_LOCAL."
#endif
