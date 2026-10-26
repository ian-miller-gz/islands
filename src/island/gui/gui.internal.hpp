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
#include <island/gui/gui.pointer.hpp>
#include <island/gui/nga.hpp>
#include <island/input.hpp>

namespace GUI {

constexpr Whole UNBORN = 0;

struct Node {
  STRING::Cold kind;
  Whole row = 0;
  STRING::Cold id;
  Whole parent = PARENTLESS;
  Properties properties;
  Flag dirty = true;
  Flag live = true;
  State state = REST;
  Flag rim = false;
};

struct Document {
  String source;
  Flag live = false;
  Whole generation = UNBORN;
  Vector<Node> nodes;
  Map<String, Whole> ids;
  Map<String, Vector<String>> styles;
  Vector<NGA::Link> links;
  Flag dirty = false;
  Vector<Whole> dirties;
  Flag restructured = false;
  Whole editing = PARENTLESS;
  Flag seated = false;
  Pointer pointer;
  INPUT::KEYS::Event keyed;
};

auto create(STRING::Hot source) -> Handle;

auto live(Handle document) -> Flag;

auto find(Handle document, STRING::Hot id, Whole &node) -> Status;

auto mark(Handle document, Whole node) -> Status;

void seat(Handle document, Whole index, Flag seated);

auto settle(const Properties &properties, Float value) -> Float;

auto fit(Float near, Float span, Float low, Float high) -> Float;

auto worn(const Document &entry, STRING::Hot name) -> Properties;

void retire(Document &entry, STRING::Hot id);

auto board(const Document &entry, Whole index) -> Whole;

auto board(const Document &entry, const NGA::Link &link) -> Whole;

auto box(const Document &entry, Whole index) -> Whole;

auto under(const Document &entry, Whole index, Whole root) -> Flag;

void clean(Handle document);

}  // namespace GUI

namespace GUI::STATE {
extern Vector<GUI::Document> documents;
extern Map<String, GFX::SPRITES::Handle> bindings;
extern Handle focused;
extern Handle claimed;
extern Whole polls;
extern Theme shade;
}  // namespace GUI::STATE
