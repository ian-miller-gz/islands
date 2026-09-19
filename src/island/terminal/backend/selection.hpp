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
#include <generated/listen.hpp>
#include <generated/network.hpp>
#include <common/platform/selection.hpp>

#if !defined(SR_LISTEN) || !defined(SR_NONE) || !defined(SR_UNIX)
#error \
  "SR_LISTEN/SR_NONE/SR_UNIX are not defined; set them in configs/make.yaml (tokens:)."
#elif SR_LISTEN != SR_NONE && SR_LISTEN != SR_UNIX
#error "SR_LISTEN has an unknown value; expected SR_NONE or SR_UNIX."
#endif

#if SR_PLATFORM == SR_WEB && SR_LISTEN != SR_NONE
#error \
  "SR_PLATFORM=SR_WEB (the browser target) has no listener; SR_LISTEN must be SR_NONE."
#endif

#if SR_PLATFORM == SR_WINDOWS && SR_LISTEN == SR_UNIX &&           \
  SR_NETWORK_BACKEND != SR_NONE && SR_NETWORK_BACKEND != SR_TCP && \
  SR_NETWORK_BACKEND != SR_TCP6 && SR_NETWORK_BACKEND != SR_TLS
#error \
  "SR_PLATFORM=SR_WINDOWS carries SR_LISTEN=SR_UNIX over an inet primary transport; SR_NETWORK_BACKEND must be SR_TCP, SR_TCP6 or SR_TLS, or SR_LISTEN must be SR_NONE."
#endif

#if SR_LISTEN == SR_UNIX && SR_NETWORK_BACKEND == SR_NONE
#error \
  "SR_LISTEN=SR_UNIX rides the stream transports; SR_NETWORK_BACKEND=SR_NONE compiles none — set SR_LISTEN: SR_NONE."
#endif
