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
#include <island/gui/dialog.internal.hpp>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace {

constexpr STRING::Hot PARENT = "..";
constexpr Char SEPARATOR = '/';

auto before(const String &left, const String &right) -> Flag {
  constexpr Whole BASE = 10;
  Whole here = 0, there = 0;
  while (here < left.size() && there < right.size()) {
    const Char one = left[here], two = right[there];
    if (!std::isdigit(one) || !std::isdigit(two)) {
      if (one != two) return one < two;
      here += 1;
      there += 1;
      continue;
    }
    Whole first = 0, second = 0;
    while (here < left.size() && std::isdigit(left[here]))
      first = first * BASE + static_cast<Whole>(left[here++] - '0');
    while (there < right.size() && std::isdigit(right[there]))
      second = second * BASE + static_cast<Whole>(right[there++] - '0');
    if (first != second) return first < second;
  }
  return left.size() - here < right.size() - there;
}

auto admits(const String &name, const String &filter) -> Flag {
  if (filter.empty()) return true;
  return name.size() >= filter.size() &&
         name.compare(name.size() - filter.size(), filter.size(), filter) == 0;
}

}  // namespace

auto GUI::DIALOG::walk(const String &directory, const String &filter)
  -> Vector<Entry> {
  Vector<Entry> places, files;
  std::error_code ec;
  if (!fs::is_directory(directory, ec)) return {};
  for (const auto &entry : fs::directory_iterator(directory, ec)) {
    const String name = entry.path().filename().string();
    if (name.empty() || name.front() == '.') continue;
    if (entry.is_directory(ec))
      places.push_back({name, true});
    else if (::admits(name, filter))
      files.push_back({name, false});
  }
  const auto reading = [](const Entry &left, const Entry &right) {
    return ::before(left.name, right.name);
  };
  std::ranges::sort(places, reading);
  std::ranges::sort(files, reading);
  Vector<Entry> rows;
  if (up(directory) != directory) rows.push_back({::PARENT, true});
  for (auto &place : places) rows.push_back(std::move(place));
  for (auto &file : files) rows.push_back(std::move(file));
  return rows;
}

auto GUI::DIALOG::join(const String &directory, const String &name) -> String {
  if (name == ::PARENT) return up(directory);
  if (directory.empty()) return name;
  const String stem =
    directory.back() == ::SEPARATOR ? directory : directory + ::SEPARATOR;
  return stem + name;
}

auto GUI::DIALOG::up(const String &directory) -> String {
  std::error_code ec;
  const auto here =
    fs::absolute(directory.empty() ? String(".") : directory, ec)
      .lexically_normal();
  const auto above = here.parent_path();
  if (ec || above.empty() || above == here) return directory;
  if (fs::path(directory).is_absolute()) return above.string();
  const auto relative = above.lexically_relative(fs::current_path(ec));
  if (ec || relative.empty() || *relative.begin() == "..")
    return above.string();
  return relative.string();
}
