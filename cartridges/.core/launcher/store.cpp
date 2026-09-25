// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>

#include <cartridge.hpp>

#include "state.hpp"

namespace fs = std::filesystem;

namespace LAUNCHER::STORE {
namespace {
constexpr Char SEPARATOR = '/';
}  // namespace
}  // namespace LAUNCHER::STORE

auto LAUNCHER::STORE::GET::list() -> String { return String(LIST); }

auto LAUNCHER::STORE::GET::home() -> String { return String(HOME); }

auto LAUNCHER::STORE::GET::leaf(const String &repository) -> String {
  const String prefix = PREFIX;
  String kept = repository;
  while (kept.size() > 1 && kept.back() == SEPARATOR) kept.pop_back();
  const auto cut = kept.find_last_of(SEPARATOR);
  const String last = cut == String::npos ? kept : kept.substr(cut + 1);
  return last.starts_with(prefix) ? last.substr(prefix.size()) : last;
}

void LAUNCHER::STORE::load() {
  offers.clear();
  IO::STREAMS::Input in(GET::list());
  String line, key, value;
  while (std::getline(in, line)) {
    const String text = LAUNCHER::GET::bare(line);
    if (text.empty() || text[0] == '#') continue;
    if (!STRING::pair(text, key, value)) continue;
    if (key == REPOSITORY && offers.size() < ROWS) {
      offers.push_back({.repository = value, .leaf = GET::leaf(value)});
      continue;
    }
    if (key == DESCRIBED && !offers.empty()) offers.back().description = value;
  }
}

void LAUNCHER::STORE::installed(Offer &offer) {
  const String home = (fs::path(GET::home()) / offer.leaf).string();
  const CARTRIDGE::Manifest manifest = CARTRIDGE::MANIFEST::read(home);
  std::error_code slip;
  offer.installed =
    !manifest.entry.empty() &&
    fs::exists(CARTRIDGE::MANIFEST::entry(home, manifest), slip);
}
