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
// The parse-check surface: one line of counts per document, or null on any
// parse/load/validate failure. Engine-free on purpose — the pytest check
// (tests/test_parse.py) drives it through ctypes, so plain C types cross
// the boundary instead of the engine's semantic aliases.
#include <cgltf/cgltf.h>

#include <cstdio>

namespace {
constexpr size_t SIZE = 256;
char text[SIZE];

size_t primitives(const cgltf_data &data) {
  size_t count = 0;
  for (size_t mesh = 0; mesh < data.meshes_count; ++mesh)
    count += data.meshes[mesh].primitives_count;
  return count;
}

size_t bytes(const cgltf_data &data) {
  size_t count = 0;
  for (size_t buffer = 0; buffer < data.buffers_count; ++buffer)
    count += data.buffers[buffer].size;
  return count;
}
}  // namespace

extern "C" const char *summary(const char *path) {
  cgltf_options options = {};
  cgltf_data *data = nullptr;
  if (cgltf_parse_file(&options, path, &data) != cgltf_result_success)
    return nullptr;
  if (
    cgltf_load_buffers(&options, data, path) != cgltf_result_success ||
    cgltf_validate(data) != cgltf_result_success) {
    cgltf_free(data);
    return nullptr;
  }
  std::snprintf(
    text, SIZE,
    "meshes %zu primitives %zu nodes %zu buffers %zu bytes %zu "
    "accessors %zu materials %zu",
    data->meshes_count, primitives(*data), data->nodes_count,
    data->buffers_count, bytes(*data), data->accessors_count,
    data->materials_count);
  cgltf_free(data);
  return text;
}
