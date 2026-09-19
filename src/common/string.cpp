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
#include <common/string.hpp>
#include <charconv>

static constexpr STRING::Hot BLANKS = " \t\r\n";

auto STRING::trim(const String &text) -> String {
  const auto first = text.find_first_not_of(BLANKS);
  if (first == String::npos) return "";
  return text.substr(first, text.find_last_not_of(BLANKS) - first + 1);
}

static auto pieces(const String &text, const String &mark, Whole width)
  -> Vector<String> {
  Vector<String> parts;
  Whole from = 0;
  for (auto at = text.find(mark); at != String::npos;
       at = text.find(mark, from)) {
    parts.push_back(text.substr(from, at - from));
    from = at + width;
  }
  parts.push_back(text.substr(from));
  return parts;
}

auto STRING::split(const String &text, Char delimiter) -> Vector<String> {
  return pieces(text, String(1, delimiter), 1);
}

auto STRING::split(const String &text, const String &separator)
  -> Vector<String> {
  return pieces(text, separator, static_cast<Whole>(separator.size()));
}

template <typename Number>
auto STRING::number(const String &text, Number fallback) -> Number {
  Number value = fallback;
  std::from_chars(text.data(), text.data() + text.size(), value);
  return value;
}

template auto STRING::number(const String &, Integer) -> Integer;
template auto STRING::number(const String &, Whole) -> Whole;
template auto STRING::number(const String &, Float) -> Float;

auto STRING::stem(const String &path) -> String {
  const auto slash = path.find_last_of('/');
  const auto from = slash == String::npos ? 0 : slash + 1;
  const auto dot = path.find_last_of('.');
  if (dot == String::npos || dot <= from) return path.substr(from);
  return path.substr(from, dot - from);
}

auto STRING::pair(const String &line, String &key, String &value) -> Flag {
  const auto colon = line.find(':');
  if (colon == String::npos) return false;
  key = trim(line.substr(0, colon));
  value = trim(line.substr(colon + 1));
  return true;
}
