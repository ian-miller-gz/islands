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
#include <test/gui/gui.internal.hpp>

Status check(STRING::Hot name, Flag passed) {
  std::cout << "gui " << name << ": " << (passed ? "PASS" : "FAIL")
            << std::endl;
  return passed ? 0 : 1;
}

static Status structure(GUI::Handle document) {
  using namespace GUI;
  Status status = check(
    "create root", NODES::create(document, NODES::ROOT, "panel", "menu") == 0);
  status |= check(
    "refuse duplicate id",
    NODES::create(document, NODES::ROOT, "panel", "menu") != 0);
  status |= check(
    "refuse empty id", NODES::create(document, NODES::ROOT, "panel", "") != 0);
  status |= check(
    "create child", NODES::create(document, "menu", "label", "title") == 0);
  status |= check(
    "refuse unknown parent",
    NODES::create(document, "ghost", "label", "stray") != 0);
  status |= check(
    "refuse unknown document",
    NODES::create(NONE, NODES::ROOT, "panel", "lost") != 0);
  return status;
}

static Status run(int count, char **arguments) {
  if (count > 1) {
    if (String(arguments[1]) == "events") return events();
    if (String(arguments[1]) == "steer") return steer();
    if (String(arguments[1]) == "edit") return edit();
    if (String(arguments[1]) == "keyed") return keyed();
    if (String(arguments[1]) == "turn") return turn();
    if (String(arguments[1]) == "graph") return graph();
    if (String(arguments[1]) == "phantom") return phantom();
    if (String(arguments[1]) == "pinned") return pinned();
    if (String(arguments[1]) == "stroked") return stroked();
    if (String(arguments[1]) == "wire") return wire();
    if (String(arguments[1]) == "route") return routed();
    if (String(arguments[1]) == "wheel") return spun();
    if (String(arguments[1]) == "pressed") return pressed();
    if (String(arguments[1]) == "bounds") return walled();
    if (String(arguments[1]) == "reborn") return reborn();
    if (String(arguments[1]) == "order") return order();
    if (String(arguments[1]) == "orphans") return orphans();
    if (String(arguments[1]) == "saved") return saved();
    if (String(arguments[1]) == "letters") return letters();
    if (String(arguments[1]) == "select") return select();
    if (String(arguments[1]) == "dialog") return picker();
    if (String(arguments[1]) == "sac") return designated();
    if (String(arguments[1]) == "ladder") return laddered();
    if (String(arguments[1]) == "window") return windowed();
    if (String(arguments[1]) == "carry") return towed();
    if (String(arguments[1]) == "stroke") return drawn();
    if (String(arguments[1]) == "strokes") return laid();
    if (String(arguments[1]) == "rows") return stacked();
    if (String(arguments[1]) == "playhead") return bladed();
    if (String(arguments[1]) == "seat") return seated();
    if (String(arguments[1]) == "beside") return beside();
    if (String(arguments[1]) == "pool") return pooled();
    if (String(arguments[1]) == "browse") return browse();
    if (String(arguments[1]) == "tip") return tip();
    if (String(arguments[1]) == "emit" && count > 2)
      return translate(arguments[2]);
    if (String(arguments[1]) == "rml" && count > 2) return cycle(arguments[2]);
    if (String(arguments[1]) == "import" && count > 2)
      return accept(arguments[2]);
    if (String(arguments[1]) == "baked" && count > 2)
      return baked(arguments[2]);
    return dump(arguments[1]);
  }
  GUI::Handle document = GUI::create("fixture");
  Status status = check("create document", document != GUI::NONE);
  status |= structure(document);
  status |= state(document);
  status |= track(document);
  status |= fields();
  status |= resize();
  status |= thumb();
  status |= styles();
  return status;
}

int main(int count, char **arguments) {
  const Status status = run(count, arguments);
  GUI::close();
  return status;
}
