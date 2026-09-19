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
#include <generated/metrics.hpp>

#if !defined(SR_METRICS) || !defined(SR_NONE) || !defined(SR_REGISTRY)
#error \
  "SR_METRICS/SR_NONE/SR_REGISTRY are not defined; set them in configs/make.yaml (tokens:)."
#elif SR_METRICS != SR_NONE && SR_METRICS != SR_REGISTRY
#error "SR_METRICS has an unknown value; expected SR_NONE or SR_REGISTRY."
#endif
