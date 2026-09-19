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
#include <cgltf/cgltf.h>
#include <island/graphics/model/model.hpp>

namespace GFX::MODEL {
using Images = Map<const cgltf_image *, GFX::Handle>;

auto load(const cgltf_data &data, const String &path, Scene &scene) -> Status;
auto load(
  const cgltf_primitive &primitive, const String &path, Images &images,
  Scene &scene) -> Status;
auto find(const cgltf_primitive &primitive, cgltf_attribute_type type)
  -> const cgltf_accessor *;
auto supported(const cgltf_primitive &primitive, const String &path) -> Flag;
auto import(const cgltf_primitive &primitive) -> Mesh;
auto texture(
  const cgltf_primitive &primitive, const String &path, Images &images,
  GFX::Handle &handle) -> Status;
void compose(const cgltf_data *data, Scene &scene);
void measure(const cgltf_data *data, Scene &scene);
}  // namespace GFX::MODEL
