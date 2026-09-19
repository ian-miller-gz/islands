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
#include <island/graphics/model/model.internal.hpp>

static void corner(
  const Float world[16], const Float point[3], GFX::MODEL::Bounds &bounds,
  Flag &begun) {
  for (Whole c = 0; c < 3; c += 1) {
    const Float placed = point[0] * world[c] + point[1] * world[4 + c] +
                         point[2] * world[8 + c] + world[12 + c];
    if (!begun || placed < bounds.min[c]) bounds.min[c] = placed;
    if (!begun || placed > bounds.max[c]) bounds.max[c] = placed;
  }
  begun = true;
}

static void extend(
  const cgltf_primitive &primitive, const Float world[16],
  GFX::MODEL::Bounds &bounds, Flag &begun) {
  const auto *accessor =
    GFX::MODEL::find(primitive, cgltf_attribute_type_position);
  if (!accessor || !accessor->has_min || !accessor->has_max) return;
  for (Whole mask = 0; mask < 8; mask += 1) {
    const Float point[3] = {
      Float((mask & 1 ? accessor->max : accessor->min)[0]),
      Float((mask & 2 ? accessor->max : accessor->min)[1]),
      Float((mask & 4 ? accessor->max : accessor->min)[2])};
    corner(world, point, bounds, begun);
  }
}

void GFX::MODEL::measure(const cgltf_data *data, Scene &scene) {
  Flag begun = false;
  for (cgltf_size n = 0; n < data->nodes_count; n += 1) {
    const auto *mesh = data->nodes[n].mesh;
    if (!mesh) continue;
    for (cgltf_size p = 0; p < mesh->primitives_count; p += 1)
      extend(mesh->primitives[p], scene.nodes[n].world, scene.bounds, begun);
  }
}
