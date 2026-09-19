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
#include <island/graphics.hpp>
#include <island/gui/events.internal.hpp>
#include <island/gui/backend.hpp>
#include <island/gui/sac.hpp>
#include <island/gui/tip.internal.hpp>
#include <logger.hpp>

namespace STATE = GUI::STATE;
namespace TIP = GUI::TIP;

namespace {

constexpr Float MARGIN = 8.0f;
constexpr Float LEAD = 14.0f;

auto stride(GUI::Handle document) -> Float {
  const Float size = GUI::GET::size(document, TIP::LABEL);
  const Float scale = size > 0.0f ? size / GFX::TEXT::EM : 1.0f;
  return GFX::TEXT::GET::advance(scale);
}

auto over(GUI::Handle document, const String &id, Float x) -> Flag {
  const Float left = GUI::GET::origin(document, id.c_str()).x;
  return x >= left && x < left + GUI::GET::measured(document, id.c_str()).w;
}

auto aim(GUI::Handle document, const String &id, Float x) -> String {
  for (const String &cell : TIP::GET::notes(document, id + "."))
    if (over(document, cell, x)) return cell;
  if (!TIP::GET::note(document, id.c_str()).empty()) return id;
  return GUI::GET::text(document, (id + TIP::HINT).c_str()).empty() ? String()
                                                                    : id;
}

auto answer(GUI::Handle document, const String &id) -> String {
  const String noted = TIP::GET::note(document, id.c_str());
  if (!noted.empty()) return noted;
  return String(GUI::GET::text(document, (id + TIP::HINT).c_str()));
}

void drop(GUI::Handle document) {
  GUI::set(document, TIP::PLATE, GUI::Visibility{false});
}

auto plate(GUI::Handle document, const String &held, Float step)
  -> GUI::Extent {
  const GUI::Position rim = GUI::GET::position(document, TIP::LABEL);
  const Float size = GUI::GET::size(document, TIP::LABEL);
  return {
    static_cast<Float>(GUI::cells(held)) * step + 2 * rim.x,
    (size > 0.0f ? size : GFX::TEXT::EM) + 2 * rim.y};
}

void raise(
  GUI::Handle document, const String &line, GUI::Extent room, Float x,
  Float y) {
  const Float step = ::stride(document);
  const Float rim = GUI::GET::position(document, TIP::LABEL).x;
  const String held =
    GUI::cut(line, GUI::budget(room.w - 2 * (MARGIN + rim), step));
  const GUI::Extent box = ::plate(document, held, step);
  GUI::set(document, TIP::LABEL, GUI::Text{held});
  GUI::set(document, TIP::PLATE, box);
  GUI::set(
    document, TIP::PLATE,
    GUI::Position{
      GUI::fit(x + LEAD, box.w, MARGIN, room.w - MARGIN),
      GUI::fit(y + LEAD, box.h, MARGIN, room.h - MARGIN)});
  GUI::set(document, TIP::PLATE, GUI::Visibility{true});
}

void watch(GUI::Handle document, GUI::Document &entry) {
  auto &pointer = entry.pointer;
  auto &rest = TIP::rested(document);
  const String hovered = pointer.hovered == GUI::PARENTLESS
                           ? String()
                           : entry.nodes[pointer.hovered].id;
  const String target =
    hovered.empty() ? String() : ::aim(document, hovered, pointer.spot.x);
  if (target != rest.topic) {
    rest.topic = target;
    rest.since = STATE::polls;
  }
  const String line = target.empty() ? String() : ::answer(document, target);
  if (line.empty() || STATE::polls - rest.since < TIP::REST) {
    ::drop(document);
    return;
  }
  ::raise(
    document, line, GUI::BACKEND::extent(document), pointer.spot.x,
    pointer.spot.y);
}

void scold(GUI::Handle document) {
  auto &rest = TIP::rested(document);
  if (rest.told) return;
  rest.told = true;
  static const String category = "~/island/gui::";
  LOGGER::get(category + "tip")
    .error(
      "A tip plate is authored but the tip is not designated: add "
      "`tip: true` under the manifest's `gui:` block (or adopt it through "
      "GUI::SAC::adopt).");
}

}  // namespace

void GUI::TIP::tend() {
  const Flag adopted = SAC::GET::adopted(TIP::NAME);
  for (Handle document = 0; document < STATE::documents.size(); document += 1) {
    auto &entry = STATE::documents[document];
    if (!entry.live) continue;
    Whole node = PARENTLESS;
    if (find(document, TIP::PLATE, node) != 0) continue;
    if (!adopted) {
      ::scold(document);
      continue;
    }
    ::watch(document, entry);
  }
}
