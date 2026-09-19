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
#include <common.hpp>
#include <iostream>

static auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto trims() -> Flag {
  return STRING::trim("  a b  ") == "a b" &&
         STRING::trim("\t\r\nx\r\n") == "x" && STRING::trim("   ").empty() &&
         STRING::trim("").empty() && STRING::trim("a") == "a";
}

static auto splits() -> Flag {
  return STRING::split("a,b,c", ',') == Vector<String>{"a", "b", "c"} &&
         STRING::split("a,,c", ',') == Vector<String>{"a", "", "c"} &&
         STRING::split(",x", ',') == Vector<String>{"", "x"} &&
         STRING::split("x,", ',') == Vector<String>{"x", ""} &&
         STRING::split("abc", ',') == Vector<String>{"abc"} &&
         STRING::split("a | b | c", " | ") == Vector<String>{"a", "b", "c"} &&
         STRING::split("a -> b", " -> ") == Vector<String>{"a", "b"} &&
         STRING::split("solo", " | ") == Vector<String>{"solo"};
}

static auto numbers() -> Flag {
  return STRING::number<Integer>("42") == 42 &&
         STRING::number<Integer>("12ab") == 12 &&
         STRING::number<Integer>("-5") == -5 &&
         STRING::number<Integer>("x", 7) == 7 &&
         STRING::number<Integer>("") == 0 &&
         STRING::number<Whole>("100") == 100u &&
         STRING::number<Float>("3.5") == 3.5f;
}

static auto stems() -> Flag {
  return STRING::stem("models/box.gltf") == "box" &&
         STRING::stem("box.gltf") == "box" && STRING::stem("cube") == "cube" &&
         STRING::stem("a/b/c") == "c" && STRING::stem(".config") == ".config" &&
         STRING::stem("dir.v2/name.ext") == "name";
}

static auto pairs() -> Flag {
  String key = "?", value = "?";
  if (STRING::pair("nocolon", key, value) || key != "?" || value != "?")
    return false;
  if (
    !STRING::pair("  a : b: c  ", key, value) || key != "a" || value != "b: c")
    return false;
  if (!STRING::pair("k:", key, value) || key != "k" || !value.empty())
    return false;
  if (!STRING::pair(": v", key, value) || !key.empty() || value != "v")
    return false;
  return true;
}

int main() {
  Status status = 0;
  status |= check("trim", trims());
  status |= check("split", splits());
  status |= check("number", numbers());
  status |= check("stem", stems());
  status |= check("pair", pairs());
  return status;
}
