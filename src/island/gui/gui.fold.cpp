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
#include <island/gui/gui.internal.hpp>

auto GUI::set(Handle document, STRING::Hot id, Fold fold) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto stand = static_cast<Whole>(fold.stand);
  if (properties.fold == stand) return 0;
  properties.fold = stand;
  return mark(document, node);
}

auto GUI::GET::fold(Handle document, STRING::Hot id) -> Fold {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto stand = STATE::documents[document].nodes[node].properties.fold;
  return {static_cast<Fold::Stand>(stand)};
}
