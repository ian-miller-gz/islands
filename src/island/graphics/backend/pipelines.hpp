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

#include <island/graphics/backend/resources.hpp>

namespace GFX {

struct Stage {
  STRING::Cold path;
  Vector<Byte> bytes;
  Whole samplers = 0;
  Vector<Whole> uniforms;
  Whole buffers = 0;
  Whole images = 0;
};

struct Group {
  Whole x = 1;
  Whole y = 1;
  Whole z = 1;
};

struct Kernel {
  Stage stage;
  Group threads = {8, 8, 1};
};

enum Topology { TRIANGLES, LINES };

struct Pipeline {
  Stage vertex;
  Stage fragment;
  Vector<Whole> attributes;
  Topology topology = TRIANGLES;
  Flag blend = true;
  Flag depth = false;
};

}  // namespace GFX

namespace GFX::PIPELINES {
auto create(const GFX::Pipeline &description) -> GFX::Handle;
void write(GFX::Handle pipeline, Whole slot, const void *data);
void destroy(GFX::Handle pipeline);
void close();
}  // namespace GFX::PIPELINES

namespace GFX::COMPUTE {
auto create(const GFX::Kernel &kernel) -> GFX::Handle;
void write(GFX::Handle kernel, Whole slot, const void *data);
void destroy(GFX::Handle kernel);
void close();
}  // namespace GFX::COMPUTE

namespace GFX {
struct Pass;
}

namespace GFX::PIPELINES::FAMILIES {
auto create(const GFX::Pipeline &description) -> GFX::Handle;
void resolve(Vector<GFX::Pass> &passes);
void destroy(GFX::Handle family);
void close();
}  // namespace GFX::PIPELINES::FAMILIES