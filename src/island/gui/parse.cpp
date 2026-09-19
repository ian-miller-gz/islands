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
#include <logger.hpp>
static const String category = "~/island/gui::";

auto GUI::PARSE::refuse(const String &message) -> Status {
  LOGGER::get(category + "parse").warn(message);
  return 1;
}

auto GUI::PARSE::parse(STRING::Hot source, const Vector<String> &lines)
  -> Handle {
  Cursor cursor{GUI::create(source)};
  for (Whole index = 0; index < lines.size(); index += 1) {
    const auto text = strip(lines[index]);
    if (text.empty()) continue;
    const auto site = String(source) + ":" + std::to_string(index + 1);
    if (interpret(cursor, text, site) != 0) {
      GUI::remove(cursor.document);
      return NONE;
    }
  }
  LOGGER::get(category + "parse").debug("Document loaded: " + String(source));
  return cursor.document;
}

static auto taken(STRING::Hot source) -> Flag {
  for (const auto &document : GUI::STATE::documents)
    if (document.live && document.source == String(source)) return true;
  return false;
}

auto GUI::load(STRING::Hot path) -> Handle {
  if (::taken(path)) {
    PARSE::refuse("The document is already loaded: " + String(path) + ".");
    return NONE;
  }
  IO::STREAMS::Input file(path);
  if (!file) {
    PARSE::refuse("Cannot read the document: " + String(path) + ".");
    return NONE;
  }
  Vector<String> lines;
  String line;
  while (std::getline(file, line)) lines.push_back(line);
  return PARSE::parse(path, lines);
}

auto GUI::load(STRING::Hot name, const Vector<Byte> &document) -> Handle {
  if (::taken(name)) {
    PARSE::refuse("The document is already loaded: " + String(name) + ".");
    return NONE;
  }
  Vector<String> lines;
  String line;
  for (const Byte byte : document) {
    if (byte == '\n') {
      lines.push_back(line);
      line.clear();
      continue;
    }
    line += byte;
  }
  if (!line.empty()) lines.push_back(line);
  return PARSE::parse(name, lines);
}
