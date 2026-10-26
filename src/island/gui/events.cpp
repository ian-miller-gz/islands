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
#include <island/gui/events.internal.hpp>
#include <island/gui/backend.hpp>
#include <island/input.hpp>

namespace STATE = GUI::STATE;

auto GUI::place(Handle document, GFX::Viewport viewport) -> Status {
  if (!live(document)) return 1;
  auto &pointer = STATE::documents[document].pointer;
  pointer.placed = true;
  pointer.placement = viewport;
  return 0;
}

auto GUI::claim(Handle document, Flag on) -> Status {
  if (!live(document)) return 1;
  if (on) {
    STATE::claimed = document;
    return 0;
  }
  if (STATE::claimed == document) STATE::claimed = NONE;
  return 0;
}

auto GUI::point(Handle document, Float u, Float v, Flag down) -> Status {
  if (!live(document)) return 1;
  auto &pointer = STATE::documents[document].pointer;
  pointer.fed = true;
  const auto extent = BACKEND::extent(document);
  const Flag inside = u >= 0.0f && u < 1.0f && v >= 0.0f && v < 1.0f &&
                      extent.w > 0.0f && extent.h > 0.0f;
  point(document, u * extent.w, v * extent.h, inside, down);
  return 0;
}

auto GUI::GET::under(Float x, Float y, Handle behind) -> Flag {
  for (Handle document = 0; document < STATE::documents.size(); document += 1) {
    if (document == behind || !live(document)) continue;
    const auto &entry = STATE::documents[document];
    if (!entry.pointer.placed) continue;
    const auto &rect = entry.pointer.placement;
    const auto extent = BACKEND::extent(document);
    if (
      rect.w <= 0.0f || rect.h <= 0.0f || extent.w <= 0.0f || extent.h <= 0.0f)
      continue;
    const Float px = (x - rect.x) / rect.w * extent.w;
    const Float py = (y - rect.y) / rect.h * extent.h;
    if (px < 0.0f || px >= extent.w || py < 0.0f || py >= extent.h) continue;
    if (BACKEND::pick(document, {px, py}) != PARENTLESS) return true;
  }
  return false;
}

struct Landed {
  Float x = 0.0f;
  Float y = 0.0f;
  Flag inside = false;
};

static auto land(
  const GFX::Viewport &rect, GUI::Extent extent, Float x, Float y) -> Landed {
  if (rect.w <= 0.0f || rect.h <= 0.0f || extent.w <= 0.0f || extent.h <= 0.0f)
    return {};
  Landed landed = {
    (x - rect.x) / rect.w * extent.w, (y - rect.y) / rect.h * extent.h};
  landed.inside = landed.x >= 0.0f && landed.x < extent.w && landed.y >= 0.0f &&
                  landed.y < extent.h;
  return landed;
}

static void sample(GUI::Handle document, const INPUT::Pointer &pointer) {
  auto &entry = STATE::documents[document];
  entry.pointer.chord = pointer.chord;
  const auto &rect = entry.pointer.placement;
  const auto extent = GUI::BACKEND::extent(document);
  const auto fold = ::land(rect, extent, pointer.x, pointer.y);
  if (pointer.rose.on && pointer.fell.on) {
    const auto &first = pointer.left ? pointer.fell : pointer.rose;
    const auto &second = pointer.left ? pointer.rose : pointer.fell;
    const auto one = ::land(rect, extent, first.x, first.y);
    const auto two = ::land(rect, extent, second.x, second.y);
    GUI::point(document, one.x, one.y, one.inside, !pointer.left);
    GUI::point(document, two.x, two.y, two.inside, pointer.left);
  } else if (pointer.rose.on || pointer.fell.on) {
    const auto &edge = pointer.rose.on ? pointer.rose : pointer.fell;
    const auto at = ::land(rect, extent, edge.x, edge.y);
    GUI::point(document, at.x, at.y, at.inside, pointer.left);
  } else {
    GUI::point(document, fold.x, fold.y, fold.inside, pointer.left);
  }
  GUI::sweep(
    document, fold.x, fold.y, fold.inside && (pointer.middle || pointer.right));
}

void GUI::poll() {
  STATE::polls += 1;
  const auto &pointer = INPUT::GET::pointer();
  for (auto &entry : STATE::documents) {
    if (!entry.live) continue;
    if (entry.pointer.staged) {
      entry.pointer.staged = false;
      continue;
    }
    entry.pointer.events.clear();
    entry.pointer.landed = false;
    entry.pointer.clicks.clear();
    entry.pointer.selections.clear();
    entry.pointer.activations.clear();
    entry.pointer.edits.clear();
    entry.pointer.commits.clear();
    entry.pointer.turns.clear();
    entry.pointer.moves.clear();
    entry.pointer.joins.clear();
    entry.pointer.cuts.clear();
    entry.pointer.ask = {};
    entry.pointer.turned = {};
    entry.pointer.spun = PARENTLESS;
    entry.pointer.press = {};
    entry.pointer.pressing = PARENTLESS;
    entry.pointer.lift = {};
    entry.pointer.lifting = PARENTLESS;
    entry.keyed = {};
  }
  navigate();
  for (Handle document = 0; document < STATE::documents.size(); document += 1) {
    auto &entry = STATE::documents[document];
    if (!entry.live) continue;
    const Flag owned = STATE::claimed == NONE || STATE::claimed == document;
    if (entry.pointer.holding) {
      GUI::point(
        document, entry.pointer.hold.x, entry.pointer.hold.y,
        entry.pointer.holdin, true);
    } else if (entry.pointer.placed && owned) {
      ::sample(document, pointer);
      if (!spin(document, pointer.wheel))
        crank(document, pointer.wheel, pointer.control);
    } else if (
      !entry.pointer.fed &&
      (entry.pointer.hovered != PARENTLESS ||
       entry.pointer.pressed != PARENTLESS || entry.pointer.down)) {
      GUI::point(document, 0.0f, 0.0f, false, false);
    }
    entry.pointer.fed = false;
  }
  bank();
  tend();
  shape();
}
