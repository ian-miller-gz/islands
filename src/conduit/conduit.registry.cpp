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
#include <utility>

auto CONDUIT::table() -> Vector<std::unique_ptr<Segment>> & {
  static Vector<std::unique_ptr<Segment>> registry;
  return registry;
}

auto CONDUIT::behind(Handle conduit) -> Segment * {
  if (conduit == NONE || conduit >= table().size()) return nullptr;
  Segment *segment = table()[conduit].get();
  return segment->base == nullptr ? nullptr : segment;
}

auto CONDUIT::seat(Segment segment) -> Handle {
  auto &registry = table();
  segment.control = static_cast<Control *>(segment.base);
  segment.slots = static_cast<unsigned char *>(segment.base) + HEADER;
  registry.push_back(std::make_unique<Segment>(std::move(segment)));
  return registry.size() - 1;
}
