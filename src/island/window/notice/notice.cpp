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
#include <island/window/notice.internal.hpp>

#include <utility>

WINDOW::NOTICE::Id WINDOW::NOTICE::STATE::standing = WINDOW::NOTICE::NONE;

auto WINDOW::notify(STRING::Hot title) -> Status {
  if (!visible) return NOTICE::UNMAPPED;
  NOTICE::Id shown = NOTICE::NONE;
  const Status status = NOTICE::show(title, NOTICE::STATE::standing, shown);
  if (status == NOTICE::SHOWN) NOTICE::STATE::standing = shown;
  return status;
}

void WINDOW::withdraw() {
  if (NOTICE::STATE::standing == NOTICE::NONE) return;
  NOTICE::close(std::exchange(NOTICE::STATE::standing, NOTICE::NONE));
}
