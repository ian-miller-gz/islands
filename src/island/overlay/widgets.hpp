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

#include <common.hpp>

namespace OVERLAY {

struct Tint {
  Float r, g, b, a = 1.0f;
};

struct Small {};
inline constexpr Small small;

void text(STRING::Hot label);
void dim(STRING::Hot label);
void tint(STRING::Hot label, Tint colour);
void wrapped(STRING::Hot label);

auto button(STRING::Hot label) -> Flag;
auto button(STRING::Hot label, Small) -> Flag;
auto selectable(STRING::Hot label, Flag selected) -> Flag;
auto checkbox(STRING::Hot label, Flag &value) -> Flag;

auto field(STRING::Hot label, String &value) -> Flag;
auto field(STRING::Hot label, Whole &value) -> Flag;
auto field(STRING::Hot label, Float &x, Float &y) -> Flag;

void sameline();
void separator();
void spacing();

void follow();

auto item(STRING::Hot label) -> Flag;
void summon(STRING::Hot name);
void dismiss();

struct Panel {
  Flag open;
  explicit Panel(STRING::Hot name);
  ~Panel();
  explicit operator bool() const { return open; }
};

struct Region {
  Flag open;
  explicit Region(STRING::Hot id);
  ~Region();
  explicit operator bool() const { return open; }
};

struct Scope {
  explicit Scope(STRING::Hot id);
  explicit Scope(Integer id);
  ~Scope();
};

struct Menubar {
  Flag open;
  Menubar();
  ~Menubar();
  explicit operator bool() const { return open; }
};

struct Menu {
  Flag open;
  explicit Menu(STRING::Hot label);
  ~Menu();
  explicit operator bool() const { return open; }
};

struct Modal {
  Flag open;
  explicit Modal(STRING::Hot name);
  ~Modal();
  explicit operator bool() const { return open; }
};

}  // namespace OVERLAY
