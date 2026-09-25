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
#include <linux/vm_sockets.h>
#include <sys/socket.h>

#include <cstdint>
#include <common.hpp>
#include <network/selection.hpp>
#include <network/types.hpp>

namespace NETWORK::VSOCK {
constexpr int FAMILY = AF_VSOCK;
constexpr Whole LOCAL = VMADDR_CID_LOCAL;
constexpr Whole ANY = VMADDR_CID_ANY;
using Sockaddr = sockaddr_vm;

inline auto resolve(const Context &context, Sockaddr &address) -> Flag {
  address.svm_family = AF_VSOCK;
  address.svm_port = static_cast<uint32_t>(context.port);
  address.svm_cid = static_cast<uint32_t>(context.cid);
  return true;
}
}  // namespace NETWORK::VSOCK
