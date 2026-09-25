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
#include <generated/network.hpp>
#include <common/platform/selection.hpp>

#if !defined(SR_NETWORK_BACKEND) || !defined(SR_UNIX) ||            \
  !defined(SR_ABSTRACT) || !defined(SR_TCP) || !defined(SR_TCP6) || \
  !defined(SR_SCTP) || !defined(SR_VSOCK) || !defined(SR_TLS) ||    \
  !defined(SR_NONE)
#error \
  "SR_NETWORK_BACKEND/SR_UNIX/SR_ABSTRACT/SR_TCP/SR_TCP6/SR_SCTP/SR_VSOCK/SR_TLS/SR_NONE are not defined; set them in configs/make.yaml (tokens:)."
#elif SR_NETWORK_BACKEND != SR_UNIX && SR_NETWORK_BACKEND != SR_ABSTRACT && \
  SR_NETWORK_BACKEND != SR_TCP && SR_NETWORK_BACKEND != SR_TCP6 &&          \
  SR_NETWORK_BACKEND != SR_SCTP && SR_NETWORK_BACKEND != SR_VSOCK &&        \
  SR_NETWORK_BACKEND != SR_TLS && SR_NETWORK_BACKEND != SR_NONE
#error \
  "SR_NETWORK_BACKEND has an unknown value; expected SR_UNIX, SR_ABSTRACT, SR_TCP, SR_TCP6, SR_SCTP, SR_VSOCK, SR_TLS, or SR_NONE."
#endif

#ifndef SR_NETWORK_BACKEND_SET
#define SR_NETWORK_BACKEND_SET (1 << SR_NETWORK_BACKEND)
#endif

#define SR_NETWORK_CARRIES(token) (SR_NETWORK_BACKEND_SET & (1 << (token)))

#if !(SR_NETWORK_CARRIES(SR_NETWORK_BACKEND))
#error "SR_NETWORK_BACKEND_SET does not carry the primary transport."
#elif SR_NETWORK_BACKEND_SET &                                             \
  ~((1 << SR_UNIX) | (1 << SR_ABSTRACT) | (1 << SR_TCP) | (1 << SR_TCP6) | \
    (1 << SR_SCTP) | (1 << SR_VSOCK) | (1 << SR_TLS) | (1 << SR_NONE))
#error "SR_NETWORK_BACKEND_SET carries an unknown transport."
#elif SR_NETWORK_CARRIES(SR_NONE) && SR_NETWORK_BACKEND_SET != (1 << SR_NONE)
#error \
  "SR_NONE is exclusive: a member that compiles nothing cannot join a set that compiles transports."
#elif (SR_NETWORK_CARRIES(SR_TCP) && SR_NETWORK_CARRIES(SR_TCP6)) || \
  (SR_NETWORK_CARRIES(SR_TCP) && SR_NETWORK_CARRIES(SR_SCTP)) ||     \
  (SR_NETWORK_CARRIES(SR_TCP6) && SR_NETWORK_CARRIES(SR_SCTP))
#error \
  "At most one inet transport may join the set: SR_TCP/SR_TCP6/SR_SCTP parameterize one shared subtree."
#endif

#if SR_PLATFORM == SR_WEB && SR_NETWORK_BACKEND != SR_NONE
#error \
  "SR_PLATFORM=SR_WEB (the browser target) opens no raw socket; SR_NETWORK_BACKEND must be SR_NONE."
#endif

#if SR_PLATFORM == SR_WINDOWS &&                                     \
  (SR_NETWORK_CARRIES(SR_UNIX) || SR_NETWORK_CARRIES(SR_ABSTRACT) || \
   SR_NETWORK_CARRIES(SR_SCTP) || SR_NETWORK_CARRIES(SR_VSOCK))
#error \
  "SR_PLATFORM=SR_WINDOWS (the Windows target) carries SR_TCP, SR_TCP6, SR_TLS or SR_NONE; SR_UNIX/SR_ABSTRACT/SR_VSOCK have no Winsock fold and Winsock has no SCTP."
#endif

#if !defined(SR_DATAGRAM_BACKEND) || !defined(SR_NONE) || !defined(SR_UDP)
#error \
  "SR_DATAGRAM_BACKEND/SR_NONE/SR_UDP are not defined; set them in configs/make.yaml (tokens:)."
#elif SR_DATAGRAM_BACKEND != SR_NONE && SR_DATAGRAM_BACKEND != SR_UDP
#error "SR_DATAGRAM_BACKEND has an unknown value; expected SR_NONE or SR_UDP."
#elif SR_PLATFORM == SR_WEB && SR_DATAGRAM_BACKEND != SR_NONE
#error \
  "SR_PLATFORM=SR_WEB (the browser target) opens no raw socket; SR_DATAGRAM_BACKEND must be SR_NONE."
#endif
