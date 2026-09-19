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
#include <filesystem>
#include <iostream>
#include <island/gui/backend/selection.hpp>
#include <logger.hpp>
#include <test/gui/gui.internal.hpp>

#if SR_GUI_BACKEND == SR_RMLUI
#include <island/gui/backend/rmlui/rmlui.internal.hpp>

namespace {

constexpr Float LINE = 16.0f;
constexpr Float ADVANCE = 8.0f;
constexpr STRING::Hot SIDECAR = "build/roundtrip.rml";

}  // namespace

Status cycle(STRING::Hot path) {
  LOGGER::initialize();
  metrics(::LINE, ::ADVANCE);
  GUI::bind("icons", 0);
  const auto original = GUI::load(path);
  if (original == GUI::NONE) return 1;
  String markup;
  if (GUI::TRANSLATE::rml(original, markup) != 0) return 1;
  std::cout << markup;
  std::error_code error;
  std::filesystem::create_directories(
    std::filesystem::path(::SIDECAR).parent_path(), error);
  IO::STREAMS::Output sidecar(::SIDECAR);
  sidecar << markup;
  sidecar.close();
  GUI::Handle imported = GUI::NONE;
  if (GUI::TRANSLATE::rml(::SIDECAR, imported) != 0) return 1;
  for (const auto &line : ::lines(imported)) std::cout << line << "\n";
  return check("import round trip", ::lines(imported) == ::lines(original));
}

Status accept(STRING::Hot path) {
  LOGGER::initialize();
  metrics(::LINE, ::ADVANCE);
  GUI::bind("icons", 0);
  GUI::Handle document = GUI::NONE;
  if (GUI::TRANSLATE::rml(path, document) != 0) return 1;
  for (const auto &node : GUI::STATE::documents[document].nodes)
    if (node.live) print(document, node);
  GUI::remove(document);
  return 0;
}

#else

Status cycle(STRING::Hot) {
  std::cout << "the RmlUi translator is not compiled (SR_GUI_BACKEND)"
            << std::endl;
  return 1;
}

Status accept(STRING::Hot) {
  std::cout << "the RmlUi translator is not compiled (SR_GUI_BACKEND)"
            << std::endl;
  return 1;
}

#endif
