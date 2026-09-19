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
#include <algorithm>

namespace NATIVE = GUI::BACKEND::NATIVE;

auto NATIVE::lettered(const Node &node, const Slot &slot) -> Float {
  const auto size = node.properties.size;
  return (size > 0 ? size / GFX::TEXT::EM : 1.0f) * slot.scale.w;
}

static void fold(
  Vector<String> &lines, String &line, const String &word, Whole budget) {
  if (line.empty()) {
    line = word;
    return;
  }
  if (UNICODE::columns(line) + 1 + UNICODE::columns(word) <= budget) {
    line += ' ';
    line += word;
    return;
  }
  lines.push_back(line);
  line = word;
}

static auto folded(const String &text, Whole columns) -> Vector<String> {
  if (!columns) return {text};
  Vector<String> lines;
  String line;
  String word;
  for (const Char letter : text) {
    if (letter != ' ') {
      word += letter;
      continue;
    }
    if (!word.empty()) ::fold(lines, line, word, columns);
    word.clear();
  }
  if (!word.empty()) ::fold(lines, line, word, columns);
  if (!line.empty()) lines.push_back(line);
  return lines;
}

static auto budget(const GUI::Node &node, const GUI::Slot &slot) -> Whole {
  const auto &properties = node.properties;
  if (properties.wrap || !GUI::KINDS::entry(node).folding)
    return properties.wrap;
  const Float advance = GFX::TEXT::GET::advance(NATIVE::lettered(node, slot));
  const Float width = slot.w - 2 * properties.pad * slot.scale.w;
  if (advance <= 0.0f || width < advance) return 1;
  return static_cast<Whole>(width / advance);
}

auto NATIVE::wrap(const Node &node, const Slot &slot) -> Vector<String> {
  const auto columns = ::budget(node, slot);
  auto lines = ::folded(node.properties.text, columns);
  const auto &entry = KINDS::entry(node);
  if (!entry.folding || node.properties.open || lines.size() <= 1) return lines;
  constexpr STRING::Hot MARK = " ...";
  constexpr Whole SPAN = 4;
  auto &line = lines.front();
  while (!line.empty() && UNICODE::columns(line) + SPAN > columns)
    line.pop_back();
  line += MARK;
  lines.resize(1);
  return lines;
}

auto NATIVE::fold(const Node &node, const Slot &slot) -> Lines {
  Lines lines;
  if (node.properties.text.empty()) return lines;
  lines.rows = wrap(node, slot);
  lines.scale = lettered(node, slot);
  lines.step = GFX::TEXT::GET::line(lines.scale);
  lines.advance = GFX::TEXT::GET::advance(lines.scale);
  lines.columns.reserve(lines.rows.size());
  for (const auto &row : lines.rows)
    lines.columns.push_back(UNICODE::columns(row));
  return lines;
}

auto NATIVE::measure(const Node &node, const Slot &slot, Float base) -> Float {
  const auto &properties = node.properties;
  if (!KINDS::entry(node).folding || !properties.open) return base;
  const auto lines = wrap(node, slot);
  const Float grown = 2 * properties.pad * slot.scale.w +
                      GFX::TEXT::GET::line(lettered(node, slot)) *
                        static_cast<Float>(lines.size());
  return std::max(base, grown);
}
