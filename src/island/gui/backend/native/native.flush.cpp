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
#include <island/gui/backend/native/native.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/text/text.hpp>
#include <island/graphics/camera.hpp>
#include <algorithm>

namespace NATIVE = GUI::BACKEND::NATIVE;

Whole GUI::STATE::writes = 0;
GFX::SPRITES::Handle GUI::STATE::white = GUI::UNBOUND;
Map<GUI::Handle, GUI::Chrome> NATIVE::mirrors;

auto NATIVE::mirror(Handle document) -> Chrome & { return mirrors[document]; }

void GUI::BACKEND::open() {
  constexpr Byte LIT = static_cast<Byte>(0xFF);
  if (GUI::STATE::white != GUI::UNBOUND) return;
  GUI::STATE::white = GFX::SPRITES::ATLASES::create(
    1, 1, {}, Vector<Byte>(GFX::TEXTURES::STRIDE, LIT));
}

auto NATIVE::crop(const Chrome &chrome, Whole clipper, const GFX::Pass &pass)
  -> GFX::Viewport {
  if (clipper == GUI::PARENTLESS) return GFX::UNCLIPPED;
  Float left = 0.0f, top = 0.0f;
  Float right = chrome.width, bottom = chrome.height;
  for (Whole at = clipper; at != GUI::PARENTLESS;
       at = chrome.slots[at].clipper) {
    const auto &slot = chrome.slots[at];
    left = std::max(left, slot.x);
    top = std::max(top, slot.y);
    right = std::min(right, slot.x + slot.w);
    bottom = std::min(bottom, slot.y + slot.h);
  }
  if (right <= left || bottom <= top) return {-1.0f, -1.0f, 0.0f, 0.0f};
  const Float scale = pass.viewport.w / chrome.width;
  return {
    pass.viewport.x + left * scale, pass.viewport.y + top * scale,
    (right - left) * scale, (bottom - top) * scale};
}

static void draw(GUI::Chrome &chrome, const GUI::Run &run, GFX::Pass &pass) {
  pass.draws.push_back(
    {.pipeline = chrome.pipelines[run.atlas],
     .vertex = {.buffers = {chrome.buffer}},
     .fragment =
       {.samplers = {GFX::SPRITES::ATLASES::GET::sampling(run.atlas)}},
     .first = run.first * GUI::VERTICES,
     .count = run.count * GUI::VERTICES,
     .scissor = NATIVE::crop(chrome, run.clipper, pass)});
}

static void append(GUI::Document &entry, GUI::Chrome &chrome, GFX::Pass &pass) {
  if (chrome.order.empty()) return;
  const auto camera = GFX::CAMERA::orthographic(
    0.0f, chrome.width, chrome.height, 0.0f, GUI::FRONT, GUI::BACK);
  for (const auto &[atlas, pipeline] : chrome.pipelines)
    GFX::PIPELINES::write(pipeline, GUI::CAMERA, &camera);
  Whole run = 0;
  for (Whole first = 0; first < chrome.order.size();) {
    const Float z = chrome.slots[chrome.order[first]].z;
    Whole last = first;
    while (last < chrome.order.size() &&
           chrome.slots[chrome.order[last]].z == z)
      last += 1;
    for (; run < chrome.runs.size() && chrome.runs[run].z == z; run += 1)
      ::draw(chrome, chrome.runs[run], pass);
    GUI::BACKEND::NATIVE::letter(entry, chrome, pass, first, last);
    first = last;
  }
}

void GUI::BACKEND::flush(Handle document, GFX::Pass &pass) {
  if (!GUI::live(document)) return;
  auto &entry = GUI::STATE::documents[document];
  auto &chrome = NATIVE::mirror(document);
  const auto scale = GFX::TEXT::GET::scale();
  const Float width = pass.viewport.w / scale;
  const Float height = pass.viewport.h / scale;
  if (
    entry.restructured || !chrome.realized || width != chrome.width ||
    height != chrome.height) {
    open();
    chrome.realized = true;
    chrome.width = width;
    chrome.height = height;
    NATIVE::compile(document, entry, chrome);
    GUI::clean(document);
  } else if (entry.dirty) {
    if (NATIVE::rewrite(document, entry, chrome) != 0)
      NATIVE::compile(document, entry, chrome);
    GUI::clean(document);
  }
  ::append(entry, chrome, pass);
}

void GUI::BACKEND::forget(Handle document) { NATIVE::mirrors.erase(document); }

void GUI::BACKEND::close() { NATIVE::mirrors.clear(); }

void GUI::BACKEND::reset() {
  NATIVE::mirrors.clear();
  GUI::STATE::white = GUI::UNBOUND;
}

auto GUI::BACKEND::writes() -> Whole { return GUI::STATE::writes; }
