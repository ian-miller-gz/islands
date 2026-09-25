// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>

#include <cartridge.hpp>

#include "state.hpp"

namespace fs = std::filesystem;
using LAUNCHER::STORE::Offer;

namespace LAUNCHER::STORE {
namespace {
constexpr STRING::Hot SOURCE = "cartridge.cpp";
constexpr STRING::Hot SWEEP = "rm -rf ";
constexpr STRING::Hot CLONE = "git clone --depth 1 ";
constexpr STRING::Hot BUILD = "bash cartridges/build.sh ";

auto staging(const Offer &offer) -> String {
  return (fs::path(LAUNCHER::STORE::STAGING) / offer.leaf).string();
}

auto placed(const Offer &offer) -> String {
  return (fs::path(LAUNCHER::STORE::GET::home()) / offer.leaf).string();
}

auto quiet(const Offer &) -> String { return {}; }

auto ended(Offer &) -> Flag { return true; }

auto cloning(const Offer &offer) -> String {
  const String home = SHELL::OS::quote(staging(offer));
  return String(SWEEP) + home + " && " + CLONE +
         SHELL::OS::quote(offer.repository) + " " + home;
}

auto building(const Offer &offer) -> String {
  return String(BUILD) + SHELL::OS::quote(staging(offer));
}

auto checked(Offer &offer) -> Flag {
  const String home = staging(offer);
  const CARTRIDGE::Manifest manifest = CARTRIDGE::MANIFEST::read(home);
  std::error_code slip;
  return manifest.name == offer.name && manifest.entry == offer.entry &&
         manifest.version == CARTRIDGE::VERSION &&
         fs::exists(fs::path(home) / SOURCE, slip);
}

auto placing(Offer &offer) -> Flag {
  const fs::path home = placed(offer);
  std::error_code slip;
  fs::remove_all(home, slip);
  fs::create_directories(home.parent_path(), slip);
  fs::rename(staging(offer), home, slip);
  if (slip) {
    fs::copy(staging(offer), home, fs::copy_options::recursive, slip);
    fs::remove_all(staging(offer), slip);
  }
  return fs::exists(home / LAUNCHER::MANIFEST, slip);
}

auto confirmed(Offer &offer) -> Flag {
  LAUNCHER::STORE::installed(offer);
  return offer.installed;
}
}  // namespace
}  // namespace LAUNCHER::STORE

auto LAUNCHER::STORE::GET::steps() -> const Vector<Step> & {
  static const Vector<Step> walk = {
    {REPORTING, quiet, report},     {VERIFYING, quiet, verify},
    {FETCHING, cloning, ended},     {CHECKING, quiet, checked},
    {BUILDING, building, ended},    {PLACING, quiet, placing},
    {CONFIRMING, quiet, confirmed},
  };
  return walk;
}
