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
#include <island/graphics/text/text.hpp>

namespace NATIVE = GUI::BACKEND::NATIVE;

static auto accent(const GUI::Node &node) -> const GFX::Color & {
  const auto &glow = node.properties.glow;
  if (node.state == GUI::REST || glow.a == 0.0f) return node.properties.edge;
  return glow;
}

static void letter(
  GUI::Document &entry, GUI::Chrome &chrome, Whole index, GFX::Pass &pass) {
  const auto &node = entry.nodes[index];
  const auto &slot = chrome.slots[index];
  const auto &lines = chrome.lines[index];
  const auto &kind = GUI::KINDS::entry(node);
  const auto ink = GUI::KINDS::washed(
    node.properties, kind.centered || kind.folding || kind.interactive
                       ? ::accent(node)
                       : node.properties.color);
  const Flag lined = kind.interactive && !kind.centered && !kind.folding;
  const Float inset =
    (kind.folding || lined ? node.properties.pad : 0.0f) * slot.scale.w;
  const auto scissor = NATIVE::crop(chrome, slot.clipper, pass);
  Float y = slot.y + (lined ? 0.0f : inset);
  if (kind.centered || lined)
    y += (slot.h - lines.step * static_cast<Float>(lines.rows.size())) / 2;
  for (Whole at = 0; at < lines.rows.size(); at += 1) {
    Float x = slot.x + inset;
    if (kind.centered)
      x += (slot.w - lines.advance * static_cast<Float>(lines.columns[at])) / 2;
    if (GFX::TEXT::draw(pass, lines.rows[at], x, y, lines.scale, ink)) {
      pass.draws.back().scissor = scissor;
      NATIVE::merge(pass.draws);
    }
    y += lines.step;
  }
}

void GUI::BACKEND::NATIVE::letter(
  Document &entry, Chrome &chrome, GFX::Pass &pass, Whole first, Whole last) {
  for (Whole at = first; at < last; at += 1) {
    const auto index = chrome.order[at];
    if (!chrome.lines[index].rows.empty()) ::letter(entry, chrome, index, pass);
    label(entry, chrome, pass, index);
  }
}
