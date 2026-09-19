// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>

#include <cartridge.hpp>

#include "state.hpp"

namespace fs = std::filesystem;
using Minutes = std::chrono::minutes;

static auto changed(const String &directory) -> String {
  std::error_code ec;
  fs::file_time_type newest = fs::file_time_type::min();
  for (const auto &at : fs::directory_iterator(directory, ec))
    if (!at.is_directory() && at.last_write_time(ec) > newest)
      newest = at.last_write_time(ec);
  if (newest == fs::file_time_type::min()) return {};
  const auto clocked = std::chrono::file_clock::to_sys(newest);
  return std::format("{:%Y-%m-%d %H:%M}", std::chrono::floor<Minutes>(clocked));
}

static auto stated(const CARTRIDGE::Manifest &manifest, STRING::Hot key)
  -> String {
  const auto found = manifest.keys.find(key);
  return found == manifest.keys.end() ? String() : found->second;
}

void LAUNCHER::identify(Entry &entry) {
  const CARTRIDGE::Manifest manifest =
    CARTRIDGE::MANIFEST::read(entry.directory);
  entry.summary = ::stated(manifest, "summary");
  entry.author = ::stated(manifest, "author");
  entry.release = ::stated(manifest, "release");
  std::error_code ec;
  entry.built =
    !manifest.entry.empty() &&
    fs::exists(CARTRIDGE::MANIFEST::entry(entry.directory, manifest), ec);
  entry.updated = ::changed(entry.directory);
}

static auto before(const String &left, const String &right) -> Flag {
  Whole here = 0, there = 0;
  while (here < left.size() && there < right.size()) {
    if (std::isdigit(left[here]) && std::isdigit(right[there])) {
      Whole first = 0, second = 0;
      for (; here < left.size() && std::isdigit(left[here]); here += 1)
        first = first * 10 + static_cast<Whole>(left[here] - '0');
      for (; there < right.size() && std::isdigit(right[there]); there += 1)
        second = second * 10 + static_cast<Whole>(right[there] - '0');
      if (first != second) return first < second;
      continue;
    }
    if (left[here] != right[there]) return left[here] < right[there];
    here += 1;
    there += 1;
  }
  return left.size() - here < right.size() - there;
}

static auto ordered(const LAUNCHER::Entry &left, const LAUNCHER::Entry &right)
  -> Flag {
  if (left.bundle != right.bundle) return right.bundle;
  return ::before(left.name, right.name);
}

static auto manifested(const fs::path &directory) -> Flag {
  std::error_code ec;
  return fs::exists(directory / LAUNCHER::MANIFEST, ec);
}

static auto leads(const fs::path &directory) -> Flag {
  if (::manifested(directory)) return true;
  std::error_code ec;
  for (const auto &at : fs::directory_iterator(directory, ec)) {
    if (!at.is_directory()) continue;
    if (at.path().filename().string().starts_with('.')) continue;
    if (::leads(at.path())) return true;
  }
  return false;
}

auto LAUNCHER::list(const String &directory, Whole depth) -> Vector<Entry> {
  Vector<Entry> rows;
  std::error_code ec;
  for (const auto &at : fs::directory_iterator(directory, ec)) {
    if (!at.is_directory()) continue;
    if (at.path().filename().string().starts_with('.')) continue;
    Entry entry{
      .name = at.path().filename().string(),
      .directory = at.path().string(),
      .depth = depth};
    entry.bundle = ::manifested(at.path());
    if (!entry.bundle && !::leads(at.path())) continue;
    if (entry.bundle) identify(entry);
    rows.push_back(std::move(entry));
  }
  std::sort(rows.begin(), rows.end(), ::ordered);
  if (!rows.empty()) rows.back().last = true;
  return rows;
}

static auto entry(const String &path) -> LAUNCHER::Entry {
  LAUNCHER::Entry row{.name = path, .directory = path};
  row.bundle = ::manifested(path);
  if (row.bundle) LAUNCHER::identify(row);
  return row;
}

void LAUNCHER::scan() {
  GUI::set(document, IDS::TREE, GUI::Cursor{0});
  GUI::set(document, IDS::TREE, GUI::Scroll{0});
  chosen.clear();
  entries = list(location, 0);
}
