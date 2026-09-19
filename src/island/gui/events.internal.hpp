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
#include <island/gui/sac.selection.hpp>
#include <island/input.hpp>

namespace GUI {

constexpr Float THRESHOLD = 3.0f;

constexpr Float REACH = 200.0f;

constexpr Float GRASP = 16.0f;

void point(Handle document, Float x, Float y, Flag inside, Flag down);

void record(Document &entry, Whole index, Event::Kind kind);

void record(Document &entry, const INPUT::KEYS::Event &event);

void grab(Document &entry, Whole hit, Float x, Float y);

void navigate();
void select(Handle document, Document &entry, Whole hit);
auto spin(Handle document, Float wheel) -> Flag;
void bank();

auto fits(Handle document, Whole node) -> Whole;

void conceal(Handle document, Whole list);
void compose(Handle document, Whole list, const String &row);

void hoist(Handle document, Whole list);

auto type(Handle document, Whole index, const INPUT::KEYS::Event &event)
  -> Flag;
void seat(Handle document, Document &entry, Whole hit);

auto typeable(const Node &node) -> Flag;

void land(Handle document, Document &entry, Whole index, Float x);
void trace(Handle document, Document &entry, Float x);

auto ring(const Document &entry, Whole index) -> Whole;

auto doubles(Pointer &aim, Whole index, Whole row) -> Flag;

auto flagged(const Properties &properties) -> Flag;
void turn(Handle document, Document &entry, Whole index, Float value);
void drag(Handle document, Document &entry, Float x, Float y);
auto flip(Handle document, Document &entry, Whole hit) -> Flag;
void restore(Handle document, Document &entry, Whole hit);
auto nudge(Handle document, Whole index, const INPUT::KEYS::Event &event)
  -> Flag;

void shift(Handle document, Document &entry, Float x, Float y);
void sweep(Handle document, Float x, Float y, Flag down);

void slide(Handle document, Document &entry, Float x, Float y);
void moor(Handle document, Document &entry);

void carry(Handle document, Document &entry, Whole index, Flag on);
auto carried() -> Flag;

void crank(Handle document, Float wheel, Flag control);

void stamp(Handle document, Document &entry, Whole hit, Float x, Float y);

void stroke(Handle document, Document &entry, Float x, Float y);

namespace NGA {
auto selectable(const Document &entry, Whole index) -> Flag;
void select(
  Handle document, Document &entry, Whole deck, const Vector<Whole> &picks);
void select(Handle document, Document &entry, Whole hit);
}  // namespace NGA

auto decked(Handle document, const Document &entry, Float x, Float y) -> Whole;
auto boxed(Handle document, const Document &entry, Whole deck, Float x, Float y)
  -> Whole;
auto under(Handle document, const Document &entry, Float x, Float y) -> Whole;

auto taken(const Document &entry, Whole hit, Whole deck) -> Flag;
void spread(Document &entry, Float x, Float y);
void gather(Handle document, Document &entry);

auto roam(Handle document, const INPUT::KEYS::Event &event) -> Flag;

void hitch(Handle document, Document &entry, Whole hit, Float x, Float y);
void trail(Handle document, Document &entry, Float x, Float y);
void fasten(Handle document, Document &entry, Whole hit);

auto snap(Handle document, const Document &entry) -> Whole;

auto wires(const Document &entry, Whole index) -> Flag;

auto aim(Handle document, const Document &entry, Whole hit, Float x, Float y)
  -> Whole;

#if SR_SAC == SR_NONE
inline void tend() {}
#else
void tend();
#endif
auto budget(Float width, Float step) -> Whole;
auto cells(const String &text) -> Whole;
auto cut(const String &text, Whole budget) -> String;

auto spanned(
  Handle document, const Document &entry, Whole deck, Float x, Float y) -> Flag;
void sever(Handle document, Document &entry, Float x, Float y);

void shape();

}  // namespace GUI
