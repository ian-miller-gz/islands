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
#include <island/gui/gui.internal.hpp>
#include <island/gui/backend.hpp>

namespace GUI {
inline const String SHADERS = COMMON::ASSETS::SHADERS + "gui";
constexpr Whole VERTICES = 6;
constexpr Whole CAMERA = 0;
constexpr Float FRONT = 0.0f;
constexpr Float BACK = -1.0f;

struct Element {
  Float x = 0.0f;
  Float y = 0.0f;
  Float w = 0.0f;
  Float h = 0.0f;
  GFX::SPRITES::Frame frame = FULL;
  GFX::Color color = GFX::COLORS::WHITE;
};

static_assert(
  sizeof(Element) == 12 * sizeof(Float),
  "the chrome element is the shader pair's 12-float ABI "
  "(assets/shaders/src/gui.vertex.hlsl)");

struct Slot {
  Whole first = 0;
  Flag shown = false;
  Float x = 0.0f, y = 0.0f;
  Float w = 0.0f, h = 0.0f;
  Float cursor = 0.0f;
  Extent scale = {1.0f, 1.0f};
  Position offset;
  Flag visible = true;
  Float z = 0.0f;
  GFX::SPRITES::Handle atlas = UNBOUND;
  Flag clip = false;
  Whole clipper = PARENTLESS;
};

struct Lines {
  Vector<String> rows;
  Vector<Whole> columns;
  Float scale = 0.0f;
  Float step = 0.0f;
  Float advance = 0.0f;
};

struct Run {
  GFX::SPRITES::Handle atlas = UNBOUND;
  Whole first = 0;
  Whole count = 0;
  Float z = 0.0f;
  Whole clipper = PARENTLESS;
};

struct Chrome {
  GFX::Handle buffer = GFX::NONE;
  Map<GFX::SPRITES::Handle, GFX::Handle> pipelines;
  Whole limit = 0;
  Vector<Whole> order;
  Vector<Slot> slots;
  Vector<Lines> lines;
  Vector<Whole> touched;
  Whole stamp = 0;
  Vector<Run> runs;
  Flag realized = false;
  Float width = 0.0f;
  Float height = 0.0f;
};

}  // namespace GUI

namespace GUI::BACKEND::NATIVE {

void compile(Handle document, Document &entry, Chrome &chrome);

auto rewrite(Handle document, Document &entry, Chrome &chrome) -> Status;

void resolve(Document &document, Chrome &chrome, Whole node);
void emit(Document &document, Chrome &chrome, Whole node);

auto stacked(const Document &document, Whole node) -> Flag;

inline auto centre(const Slot &slot) -> Position {
  return {slot.x + slot.w / 2.0f, slot.y + slot.h / 2.0f};
}

void wire(Document &document, Chrome &chrome, Whole board);

void guide(
  const Document &document, const Chrome &chrome, Whole board, Element *seat);

void label(Document &document, Chrome &chrome, GFX::Pass &pass, Whole board);

void batch(Document &document, Chrome &chrome);

void letter(
  Document &document, Chrome &chrome, GFX::Pass &pass, Whole first, Whole last);

void merge(Vector<GFX::Draw> &draws);

auto crop(const Chrome &chrome, Whole clipper, const GFX::Pass &pass)
  -> GFX::Viewport;

auto referent(const Document &entry, Whole index) -> Whole;
auto ordering(const Document &entry) -> Vector<Whole>;

auto wrap(const Node &node, const Slot &slot) -> Vector<String>;
auto measure(const Node &node, const Slot &slot, Float base) -> Float;

auto fold(const Node &node, const Slot &slot) -> Lines;

auto lettered(const Node &node, const Slot &slot) -> Float;

}  // namespace GUI::BACKEND::NATIVE

namespace GUI::STATE {
extern Whole writes;
extern GFX::SPRITES::Handle white;
}  // namespace GUI::STATE
