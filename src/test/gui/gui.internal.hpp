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
#include <island/gui/dialog.hpp>
#include <island/gui/gui.internal.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/sac.hpp>
#include <island/input.hpp>

Status check(STRING::Hot name, Flag passed);
Status state(GUI::Handle document);
Status track(GUI::Handle document);
Status fields();
Status styles();
Status resize();
Status thumb();
Status events();
Status shadowed();
Status pinned();
Status stroked();
Status dump(STRING::Hot path);
Status translate(STRING::Hot path);
Status cycle(STRING::Hot path);
Status accept(STRING::Hot path);
Status baked(STRING::Hot path);
void print(GUI::Handle document, const GUI::Node &node);
auto describe(GUI::Handle document, const GUI::Node &node) -> String;
auto lines(GUI::Handle document) -> Vector<String>;

void rescale(Float scale);
void metrics(Float line, Float advance);

struct Inked {
  String text;
  Float x = 0.0f;
  Float y = 0.0f;
  Float scale = 0.0f;
};
auto inked() -> Vector<Inked>;

struct Worn {
  Whole w = 0;
  Whole h = 0;
  Whole x = 0;
  Whole y = 0;
};
auto worn() -> Worn;
void aim(Float x, Float y);
void hold(Flag down);
void grip(Flag down);
void chord(Flag control, Flag shift, Flag alt);
void flick();
void flick(Float x, Float y);

void typed(const Vector<INPUT::KEYS::Event> &events);
void tap(Whole codepoint);
Status picker();
Status designated();
Status laddered();
Status windowed();
Status drawn();
Status towed();
Status ridden();
Status stacked();
Status bladed();
Status seated();

Status pooled();

Status hidden();

Status beside();

Status laid();

void wheeled(Float amount, Flag control);

void clipped(const String &text);
auto clipping() -> String;
Status steer();

Status keyed();

Status edit();

Status turn();

Status graph();

Status phantom();

Status spun();

Status pressed();

Status walled();

Status stretched(GUI::Handle bench);

Status reborn();

Status letters();

Status wire();

Status routed();

Status saved();

Status order();

Status orphans();

Status select();

Status browse();

Status tip();

extern GUI::Handle deck;
extern GUI::Handle spare;
constexpr GFX::Viewport WHOLE = {0.0f, 0.0f, 640.0f, 480.0f};
Status grow();
void step(STRING::Hot name);

Status travel();
