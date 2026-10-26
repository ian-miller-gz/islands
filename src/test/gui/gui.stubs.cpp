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
#include <test/gui/gui.internal.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/sprites/sprites.hpp>
#include <island/graphics/text/text.hpp>
#include <island/graphics/windows.hpp>
#include <island/input.hpp>
#include <island/window/backend.hpp>
#include <island/graphics/capture.hpp>

static Map<GFX::Handle, Vector<Byte>> buffers;
static GFX::Handle handles = 0;
static Float scaled = 1.0f;
static Float lined = 0.0f;
static Float advanced = 0.0f;
static INPUT::Pointer pointer;
static Vector<INPUT::KEYS::Event> composed;
static Whole tapped = 0;
static Flag wants = false;

void rescale(Float scale) { scaled = scale; }

void metrics(Float line, Float advance) {
  lined = line;
  advanced = advance;
}

void aim(Float x, Float y) {
  pointer.x = x;
  pointer.y = y;
  pointer.rose = {};
  pointer.fell = {};
}

void hold(Flag down) {
  pointer.left = down;
  pointer.rose = {};
  pointer.fell = {};
}

void grip(Flag down) { pointer.middle = down; }

void chord(Flag control, Flag shift, Flag alt) {
  pointer.chord = {control, shift, alt};
}

void flick() { flick(pointer.x, pointer.y); }

void flick(Float x, Float y) {
  pointer.left = false;
  pointer.rose = {true, x, y};
  pointer.fell = {true, x, y};
}

void typed(const Vector<INPUT::KEYS::Event> &events) { composed = events; }

void wheeled(Float amount, Flag control) {
  pointer.wheel = amount;
  pointer.control = control;
}

void tap(Whole codepoint) { tapped = codepoint; }

auto INPUT::GET::pointer() -> const INPUT::Pointer & { return ::pointer; }

static WINDOW::Image carried;
static Whole hotx = 0;
static Whole hoty = 0;

void WINDOW::BACKEND::shape(Shape) {
  ::carried = {};
  ::hotx = 0;
  ::hoty = 0;
}

void WINDOW::BACKEND::wear(const Image &image, Whole hotx, Whole hoty) {
  if (!image.w || !image.h) return;
  if (image.pixels.size() < image.w * image.h * CHANNELS) return;
  ::carried = image;
  ::hotx = hotx < image.w ? hotx : image.w - 1;
  ::hoty = hoty < image.h ? hoty : image.h - 1;
}

auto WINDOW::BACKEND::GET::worn() -> Flag { return ::carried.w != 0; }

auto worn() -> Worn { return {::carried.w, ::carried.h, ::hotx, ::hoty}; }

auto GFX::CAPTURE::read(Whole, Whole, Whole w, Whole h) -> WINDOW::Image {
  if (!w || !h) return {};
  return {w, h, Vector<Byte>(w * h * WINDOW::CHANNELS, 0)};
}

auto INPUT::GET::pressed(Whole codepoint) -> Flag {
  return codepoint != 0 && tapped == codepoint;
}

void INPUT::TEXT::focus(Flag on) { wants = on; }

auto INPUT::TEXT::wanted() -> Flag { return wants; }

void INPUT::TEXT::claim(Flag owned) {
  if (!owned) return;
  wants = false;
  composed.clear();
}

static String board;

void clipped(const String &text) { board = text; }

auto clipping() -> String { return ::board; }

void INPUT::CLIPBOARD::write(const String &text) { ::board = text; }

auto INPUT::CLIPBOARD::read() -> String { return ::board; }

auto INPUT::TEXT::drain() -> Vector<INPUT::KEYS::Event> {
  const auto events = composed;
  composed.clear();
  return wants ? events : Vector<INPUT::KEYS::Event>{};
}

auto GFX::PIPELINES::create(const GFX::Pipeline &) -> GFX::Handle {
  return GFX::NONE;
}

void GFX::PIPELINES::write(GFX::Handle, Whole, const void *) {}

auto GFX::BUFFERS::create(Whole size, Kind) -> GFX::Handle {
  buffers[handles].assign(size, 0);
  handles += 1;
  return handles - 1;
}

void GFX::BUFFERS::destroy(GFX::Handle buffer) { buffers.erase(buffer); }

auto GFX::BUFFERS::GET::data(GFX::Handle buffer) -> void * {
  const auto found = buffers.find(buffer);
  return found == buffers.end() ? nullptr : found->second.data();
}

auto GFX::SPRITES::ATLASES::create(Whole, Whole, Cell, const Vector<Byte> &)
  -> Handle {
  return 0;
}

auto GFX::SPRITES::ATLASES::frame(Handle, Whole column, Whole row) -> Frame {
  return {static_cast<Float>(column), static_cast<Float>(row), 1.0f, 1.0f};
}

auto GFX::SPRITES::ATLASES::GET::extent(Handle) -> Extent { return {1, 1}; }

auto GFX::SPRITES::ATLASES::GET::sampling(Handle) -> GFX::Sampling {
  return {};
}

static Vector<Inked> inks;

auto GFX::TEXT::draw(
  GFX::Pass &, const String &text, Float x, Float y, Float scale,
  const GFX::Color &) -> Whole {
  inks.push_back({text, x, y, scale});
  return 0;
}

auto inked() -> Vector<Inked> {
  const auto runs = ::inks;
  ::inks.clear();
  return runs;
}

auto GFX::TEXT::GET::scale() -> Float { return scaled; }

auto GFX::TEXT::GET::line(Float scale) -> Float { return ::lined * scale; }

auto GFX::TEXT::GET::advance(Float scale) -> Float {
  return ::advanced * scale;
}

auto GFX::TEXT::GET::glyph(Whole, Float) -> const GFX::TEXT::Glyph * {
  return nullptr;
}

auto GFX::TEXT::GET::font(Float) -> const GFX::TEXT::Font & {
  static const GFX::TEXT::Font empty;
  return empty;
}

auto GFX::TEXT::GET::sampling() -> GFX::Sampling { return {}; }

auto GFX::WINDOWS::MAIN::viewport() -> GFX::Viewport { return {}; }

void GFX::PASSES::submit(GFX::Pass &&) {}
