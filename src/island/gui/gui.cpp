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
#include <island/gui/backend.hpp>
#include <logger.hpp>
static const String category = "~/island/gui::";

Vector<GUI::Document> GUI::STATE::documents;
Map<String, GFX::SPRITES::Handle> GUI::STATE::bindings;
GUI::Handle GUI::STATE::focused = GUI::NONE;
GUI::Handle GUI::STATE::claimed = GUI::NONE;
Whole GUI::STATE::polls = 0;
GUI::Theme GUI::STATE::shade = GUI::LIGHT;

auto GUI::create(STRING::Hot source) -> Handle {
  STATE::documents.push_back({source, true, UNBORN + 1});
  return static_cast<Handle>(STATE::documents.size() - 1);
}

void GUI::bind(STRING::Hot name, GFX::SPRITES::Handle atlas) {
  STATE::bindings[name] = atlas;
}

void GUI::remove(Handle document) {
  if (document >= STATE::documents.size()) return;
  if (STATE::focused == document) focus(document, false);
  if (STATE::claimed == document) STATE::claimed = NONE;
  BACKEND::forget(document);
  STATE::documents[document] = {};
}

void GUI::close() {
  LOGGER::get(category + "gui").debug("Closing GUI...");
  reset();
  BACKEND::close();
}

void GUI::reset() {
  LOGGER::get(category + "gui").debug("Dropping GUI documents...");
  if (STATE::focused != NONE) focus(STATE::focused, false);
  STATE::claimed = NONE;
  STATE::documents.clear();
  STATE::bindings.clear();
  BACKEND::reset();
}

auto GUI::GET::document(STRING::Hot source) -> Handle {
  const String name(source);
  if (name.empty()) return NONE;
  for (Whole handle = 0; handle < STATE::documents.size(); handle += 1) {
    const auto &entry = STATE::documents[handle];
    if (!entry.live) continue;
    if (entry.source == name) return handle;
    if (
      entry.source.size() > name.size() && entry.source.ends_with(name) &&
      entry.source[entry.source.size() - name.size() - 1] == '/')
      return handle;
  }
  return NONE;
}

auto GUI::GET::document() -> Handle {
  if (STATE::focused != NONE && live(STATE::focused)) return STATE::focused;
  if (STATE::claimed != NONE && live(STATE::claimed)) return STATE::claimed;
  return NONE;
}
