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
#include <iostream>
#include <island/gui/parse.internal.hpp>
#include <island/gui/translate.hpp>
#include <logger.hpp>
#include <test/gui/gui.internal.hpp>

Status fields() {
  using namespace GUI::PARSE;
  Vector<String> words;
  Status status = check(
    "split binds quotes", split("label a text=\"Set sail\" x=4", words) == 0 &&
                            words.size() == 4 && words[2] == "text=Set sail");
  status |= check(
    "split refuses unterminated quote",
    split("label a text=\"Set sail", words) != 0);
  status |= check(
    "strip keeps colors, drops comments",
    strip("panel p color=#11223344 # note") == "panel p color=#11223344" &&
      strip("# a whole-line comment").empty());
  return status;
}

Status dump(STRING::Hot path) {
  LOGGER::initialize();
  GUI::bind("icons", 0);
  const auto document = GUI::load(path);
  if (document == GUI::NONE) return 1;
  for (const auto &node : GUI::STATE::documents[document].nodes)
    if (node.live) print(document, node);
  for (const auto &link : GUI::NGA::GET::links(document, ""))
    std::cout << "link " << link.from << " " << link.to << " \"" << link.label
              << "\"" << std::endl;
  GUI::remove(document);
  return GUI::STATE::documents[document].live ? 1 : 0;
}

Status translate(STRING::Hot path) {
  LOGGER::initialize();
  GUI::bind("icons", 0);
  const auto document = GUI::load(path);
  if (document == GUI::NONE) return 1;
  String text;
  const auto status = GUI::TRANSLATE::emit(document, text);
  if (status == 0) std::cout << text;
  GUI::remove(document);
  return status;
}
