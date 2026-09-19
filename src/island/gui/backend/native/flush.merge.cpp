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
#include <island/gui/backend/native/native.internal.hpp>
#include <island/graphics/backend/passes.hpp>

void GUI::BACKEND::NATIVE::merge(Vector<GFX::Draw> &draws) {
  if (draws.size() < 2) return;
  const auto &last = draws.back();
  auto &prior = draws[draws.size() - 2];
  if (prior.first + prior.count != last.first) return;
  if (prior.pipeline != last.pipeline || prior.vertices != last.vertices)
    return;
  if (prior.indices != GFX::NONE || last.indices != GFX::NONE) return;
  if (!prior.uniforms.empty() || !last.uniforms.empty()) return;
  if (!(prior.scissor == last.scissor)) return;
  if (!(prior.vertex == last.vertex) || !(prior.fragment == last.fragment))
    return;
  prior.count += last.count;
  draws.pop_back();
}
