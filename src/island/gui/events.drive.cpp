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

namespace STATE = GUI::STATE;

namespace {

struct Aim {
  Float x = 0.0f;
  Float y = 0.0f;
  Flag inside = false;
};

auto aimed(GUI::Handle document, Whole node, Aim &aim) -> Status {
  const auto extent = GUI::BACKEND::extent(document);
  if (extent.w <= 0.0f || extent.h <= 0.0f) return 1;
  const auto corner = GUI::BACKEND::origin(document, node);
  const auto size = GUI::BACKEND::measured(document, node);
  aim.x = corner.x + size.w / 2.0f;
  aim.y = corner.y + size.h / 2.0f;
  aim.inside =
    aim.x >= 0.0f && aim.x < extent.w && aim.y >= 0.0f && aim.y < extent.h;
  return 0;
}

auto landed(GUI::Handle document, STRING::Hot id, Aim &aim) -> Status {
  if (!GUI::live(document)) return 1;
  Whole node = GUI::PARENTLESS;
  if (GUI::find(document, id, node) != 0) return 1;
  return aimed(document, node, aim);
}

void feed(
  GUI::Handle document, const Aim &start, const Aim &end, Whole holds,
  const INPUT::Chord &chord) {
  STATE::documents[document].pointer.fed = true;
  STATE::documents[document].pointer.chord = chord;
  GUI::point(document, start.x, start.y, start.inside, false);
  GUI::point(document, start.x, start.y, start.inside, true);
  for (Whole sample = 0; sample < holds; sample += 1)
    GUI::point(document, end.x, end.y, end.inside, true);
  GUI::point(document, end.x, end.y, end.inside, false);
}

}  // namespace

auto GUI::stage(Handle document) -> Status {
  if (!live(document)) return 1;
  auto &pointer = STATE::documents[document].pointer;
  if (pointer.staged) return 0;
  pointer.events.clear();
  pointer.clicks.clear();
  pointer.selections.clear();
  pointer.activations.clear();
  pointer.edits.clear();
  pointer.commits.clear();
  pointer.turns.clear();
  pointer.moves.clear();
  pointer.joins.clear();
  pointer.cuts.clear();
  pointer.ask = {};
  pointer.turned = {};
  pointer.spun = PARENTLESS;
  pointer.press = {};
  pointer.pressing = PARENTLESS;
  pointer.lift = {};
  pointer.lifting = PARENTLESS;
  pointer.staged = true;
  return 0;
}

auto GUI::click(Handle document, STRING::Hot id) -> Status {
  Aim aim;
  if (::landed(document, id, aim) != 0) return 1;
  ::feed(document, aim, aim, 0, {});
  return 0;
}

auto GUI::doubleclick(Handle document, STRING::Hot id) -> Status {
  Aim aim;
  if (::landed(document, id, aim) != 0) return 1;
  ::feed(document, aim, aim, 0, {});
  ::feed(document, aim, aim, 0, {});
  return 0;
}

auto GUI::press(
  Handle document, STRING::Hot id, Float x, Float y,
  const INPUT::Chord &chord) -> Status {
  Aim aim;
  if (::landed(document, id, aim) != 0) return 1;
  const auto extent = GUI::BACKEND::extent(document);
  aim.x += x;
  aim.y += y;
  aim.inside =
    aim.x >= 0.0f && aim.x < extent.w && aim.y >= 0.0f && aim.y < extent.h;
  auto &pointer = STATE::documents[document].pointer;
  pointer.fed = true;
  pointer.chord = chord;
  GUI::point(document, aim.x, aim.y, aim.inside, false);
  GUI::point(document, aim.x, aim.y, aim.inside, true);
  pointer.holding = true;
  pointer.hold = {aim.x, aim.y};
  pointer.holdin = aim.inside;
  return 0;
}

auto GUI::ask(Handle document, STRING::Hot id, Float x, Float y) -> Status {
  Aim aim;
  if (::landed(document, id, aim) != 0) return 1;
  const auto extent = GUI::BACKEND::extent(document);
  aim.x += x;
  aim.y += y;
  aim.inside =
    aim.x >= 0.0f && aim.x < extent.w && aim.y >= 0.0f && aim.y < extent.h;
  if (!aim.inside) return 1;
  GUI::sweep(document, aim.x, aim.y, true);
  GUI::sweep(document, aim.x, aim.y, false);
  return STATE::documents[document].pointer.ask.asked ? 0 : 1;
}

auto GUI::release(Handle document, Float x, Float y) -> Status {
  if (!live(document)) return 1;
  auto &pointer = STATE::documents[document].pointer;
  if (!pointer.holding) return 1;
  const auto extent = GUI::BACKEND::extent(document);
  Aim end;
  end.x = pointer.hold.x + x;
  end.y = pointer.hold.y + y;
  end.inside =
    end.x >= 0.0f && end.x < extent.w && end.y >= 0.0f && end.y < extent.h;
  pointer.fed = true;
  if (x != 0.0f || y != 0.0f)
    GUI::point(document, end.x, end.y, end.inside, true);
  GUI::point(document, end.x, end.y, end.inside, false);
  pointer.holding = false;
  return 0;
}

auto GUI::travel(Handle document, Float x, Float y) -> Status {
  if (!live(document)) return 1;
  auto &pointer = STATE::documents[document].pointer;
  if (!pointer.holding) return 1;
  const auto extent = GUI::BACKEND::extent(document);
  Aim leg;
  leg.x = pointer.hold.x + x;
  leg.y = pointer.hold.y + y;
  leg.inside =
    leg.x >= 0.0f && leg.x < extent.w && leg.y >= 0.0f && leg.y < extent.h;
  pointer.fed = true;
  GUI::point(document, leg.x, leg.y, leg.inside, true);
  pointer.hold = {leg.x, leg.y};
  pointer.holdin = leg.inside;
  return 0;
}

auto GUI::drag(
  Handle document, STRING::Hot from, STRING::Hot to,
  const INPUT::Chord &chord) -> Status {
  Aim start, end;
  if (::landed(document, from, start) != 0) return 1;
  if (::landed(document, to, end) != 0) return 1;
  ::feed(document, start, end, 1, chord);
  return 0;
}

auto GUI::wheel(Handle document, STRING::Hot id, Float turns, Flag control)
  -> Status {
  Aim aim;
  if (::landed(document, id, aim) != 0) return 1;
  STATE::documents[document].pointer.fed = true;
  point(document, aim.x, aim.y, aim.inside, false);
  if (!spin(document, turns)) crank(document, turns, control);
  return 0;
}

auto GUI::drag(
  Handle document, STRING::Hot id, Float x, Float y, Whole holds,
  const INPUT::Chord &chord) -> Status {
  Aim start;
  if (::landed(document, id, start) != 0) return 1;
  const auto extent = GUI::BACKEND::extent(document);
  Aim end;
  end.x = start.x + x;
  end.y = start.y + y;
  end.inside =
    end.x >= 0.0f && end.x < extent.w && end.y >= 0.0f && end.y < extent.h;
  ::feed(document, start, end, holds == 0 ? 1 : holds, chord);
  return 0;
}
