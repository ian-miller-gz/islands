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
#include <island/gui/gui.internal.hpp>
#include <island/gui/parse.internal.hpp>
#include <island/gui/backend.hpp>

static void rebuild(GUI::Handle document) {
  auto &entry = GUI::STATE::documents[document];
  const String source = entry.source;
  const Whole generation = entry.generation;
  IO::STREAMS::Input file(source);
  if (!file) {
    GUI::remove(document);
    return;
  }
  Vector<String> lines;
  String line;
  while (std::getline(file, line)) lines.push_back(line);
  if (GUI::STATE::focused == document) GUI::focus(document, false);
  if (GUI::STATE::claimed == document) GUI::STATE::claimed = GUI::NONE;
  GUI::BACKEND::forget(document);
  entry = {};
  entry.source = source;
  entry.live = true;
  entry.generation = generation + 1;
  GUI::PARSE::Cursor cursor{document};
  for (Whole index = 0; index < lines.size(); index += 1) {
    const auto text = GUI::PARSE::strip(lines[index]);
    if (text.empty()) continue;
    const auto site = source + ":" + std::to_string(index + 1);
    if (GUI::PARSE::interpret(cursor, text, site) != 0) {
      GUI::remove(document);
      return;
    }
  }
}

void GUI::theme(Theme shade) {
  if (STATE::shade == shade) return;
  STATE::shade = shade;
  for (Whole handle = 0; handle < STATE::documents.size(); handle += 1)
    if (STATE::documents[handle].live) ::rebuild(handle);
}

auto GUI::GET::theme() -> Theme { return STATE::shade; }
