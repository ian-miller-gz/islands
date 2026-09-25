// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>

#include <cartridge.hpp>

#include "state.hpp"

namespace fs = std::filesystem;
using LAUNCHER::STORE::Offer;

namespace LAUNCHER::STORE {
namespace {
constexpr STRING::Hot CONTRACT = "v";

auto address(const String &repository) -> String {
  namespace STORE = LAUNCHER::STORE;
  const String forge = STORE::FORGE;
  if (repository.starts_with(forge))
    return STORE::RAW + repository.substr(forge.size()) + STORE::REVISION +
           LAUNCHER::MANIFEST;
  return (fs::path(repository) / LAUNCHER::MANIFEST).string();
}

auto fetched(const Offer &offer, String &report) -> Flag {
  const String from = address(offer.repository);
  if (offer.repository.find(LAUNCHER::STORE::SCHEME) == String::npos) {
    IO::STREAMS::Input in(from);
    if (!in) return false;
    report.assign(std::istreambuf_iterator<Char>(in), {});
    return true;
  }
  const NETWORK::WEB::Result answer = NETWORK::WEB::get(from);
  report = answer.body;
  return answer.ok;
}

auto staging(const Offer &offer) -> String {
  return (fs::path(LAUNCHER::STORE::STAGING) / offer.leaf).string();
}
}  // namespace
}  // namespace LAUNCHER::STORE

auto LAUNCHER::STORE::report(Offer &offer) -> Flag {
  String text;
  offer.state.clear();
  if (!fetched(offer, text)) {
    offer.state = UNREACHED;
    return false;
  }
  const fs::path home = staging(offer);
  std::error_code slip;
  fs::create_directories(home, slip);
  IO::STREAMS::Output out((home / LAUNCHER::MANIFEST).string());
  out << text;
  return out.good();
}

auto LAUNCHER::STORE::verify(Offer &offer) -> Flag {
  const CARTRIDGE::Manifest manifest =
    CARTRIDGE::MANIFEST::read(staging(offer));
  if (
    manifest.name.empty() || manifest.entry.empty() ||
    manifest.version != CARTRIDGE::VERSION) {
    offer.state = REFUSED;
    return false;
  }
  const auto found = manifest.keys.find(RELEASED);
  offer.name = manifest.name;
  offer.entry = manifest.entry;
  offer.release = found != manifest.keys.end()
                    ? found->second
                    : CONTRACT + std::to_string(manifest.version);
  return true;
}

void LAUNCHER::STORE::survey() {
  if (surveyed) return;
  surveyed = true;
  load();
  for (Offer &offer : offers)
    if (report(offer)) verify(offer);
}
