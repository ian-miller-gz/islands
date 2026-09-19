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
#include <iostream>
#include <network.hpp>

int main() {
#if SR_DATAGRAM_BACKEND == SR_UDP
  using namespace NETWORK;
  const Socket first{"127.0.0.1", 51413}, second{"127.0.0.1", 51414};
  const String message = "islands-datagram-round-trip";
  Handle sender = DATAGRAM::open(first), receiver = DATAGRAM::open(second);
  if (sender == NONE || receiver == NONE) {
    std::cout << "datagram: open failed" << std::endl;
    return 1;
  }
  Socket origin{};
  String received;
  bool delivered = DATAGRAM::emit(sender, second, message) &&
                   DATAGRAM::receive(receiver, origin, received);
  DATAGRAM::close(sender);
  DATAGRAM::close(receiver);
  bool ok = delivered && received == message && origin.port == first.port;
  std::cout << "datagram round-trip: " << (ok ? "PASS" : "FAIL") << std::endl;
  return ok ? 0 : 1;
#else
  return 0;
#endif
}
