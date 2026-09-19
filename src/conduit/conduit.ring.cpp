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
#include "conduit.internal.hpp"
#include <cstring>

using CONDUIT::Control;
using CONDUIT::Segment;

auto CONDUIT::write(Handle conduit, const void *record) -> Flag {
  Segment *segment = behind(conduit);
  if (segment == nullptr) return false;
  Control &control = *segment->control;
  const uint64_t tail = control.tail.load(std::memory_order_relaxed);
  const uint64_t head = control.head.load(std::memory_order_acquire);
  if (tail - head >= control.records) return false;
  const uint64_t slot = tail & (control.records - 1);
  std::memcpy(segment->slots + slot * control.stride, record, control.stride);
  control.tail.store(tail + 1, std::memory_order_release);
  return true;
}

auto CONDUIT::read(Handle conduit, void *record) -> Flag {
  Segment *segment = behind(conduit);
  if (segment == nullptr) return false;
  Control &control = *segment->control;
  const uint64_t head = control.head.load(std::memory_order_relaxed);
  const uint64_t tail = control.tail.load(std::memory_order_acquire);
  if (head == tail) return false;
  const uint64_t slot = head & (control.records - 1);
  std::memcpy(record, segment->slots + slot * control.stride, control.stride);
  control.head.store(head + 1, std::memory_order_release);
  return true;
}

auto CONDUIT::GET::records(Handle conduit) -> Whole {
  Segment *segment = behind(conduit);
  return segment == nullptr ? 0 : segment->control->records;
}

auto CONDUIT::GET::stride(Handle conduit) -> Whole {
  Segment *segment = behind(conduit);
  return segment == nullptr ? 0 : segment->control->stride;
}
