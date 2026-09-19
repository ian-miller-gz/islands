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
#include <island/gui/backend/native/kinds.internal.hpp>
#include <island/gui/nga.internal.hpp>
#include <island/graphics/text/text.hpp>

namespace NGA = GUI::NGA;
namespace NATIVE = GUI::BACKEND::NATIVE;

namespace {

void say(
  GUI::Document &entry, GUI::Chrome &chrome, const NGA::Link &link, Whole index,
  GFX::Pass &pass) {
  const auto source = entry.ids.find(link.from);
  const auto sink = entry.ids.find(link.to);
  if (source == entry.ids.end() || sink == entry.ids.end()) return;
  const auto &carried = chrome.slots[source->second];
  const auto out = NATIVE::centre(carried);
  const auto in = NATIVE::centre(chrome.slots[sink->second]);
  const Float scale = NATIVE::lettered(entry.nodes[index], carried);
  const String label = link.label;
  const Float span = GFX::TEXT::GET::advance(scale) *
                     static_cast<Float>(UNICODE::columns(label));
  const Float x = (out.x + in.x) / 2.0f - span / 2.0f;
  const Float y = (out.y + in.y) / 2.0f - GFX::TEXT::GET::line(scale) / 2.0f;
  const auto scissor = NATIVE::crop(chrome, carried.clipper, pass);
  if (GFX::TEXT::draw(
        pass, label, x, y, scale,
        entry.nodes[source->second].properties.edge)) {
    pass.draws.back().scissor = scissor;
    NATIVE::merge(pass.draws);
  }
}

}  // namespace

void GUI::BACKEND::NATIVE::label(
  Document &entry, Chrome &chrome, GFX::Pass &pass, Whole index) {
  if (entry.nodes[index].row != KINDS::GRAPH || !chrome.slots[index].shown)
    return;
  for (const auto &link : entry.links)
    if (!link.label.empty() && board(entry, link) == index)
      ::say(entry, chrome, link, index, pass);
}
