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
#include <cartridge/axes.internal.hpp>
#include <cartridge/manifest.hpp>
#include <cartridge/manifest.internal.hpp>

#include <generated/compatibility.hpp>

namespace {
using CARTRIDGE::Manifest;
using CARTRIDGE::AXES::Entry;

struct Rule {
  STRING::Hot setting;
  STRING::Hot value;
  STRING::Hot key;
  STRING::Hot admits;
  STRING::Hot refuses;
};

constexpr Rule RULES[] = {SR_COMPATIBILITIES{"", "", "", "", ""}};

auto listed(const String &joined, const String &spelling) -> Flag {
  for (const auto &member : STRING::split(joined, ','))
    if (STRING::trim(member) == spelling) return true;
  return false;
}

auto explain(const String &axis, const String &want) -> const Rule * {
  for (const auto &rule : RULES) {
    if (axis != rule.key) continue;
    const Entry *setting = CARTRIDGE::AXES::find(rule.setting);
    if (!setting) continue;
    const Whole held = CARTRIDGE::AXES::token(*setting, rule.value);
    if (!held || !CARTRIDGE::AXES::carries(*setting, held)) continue;
    const String admits = rule.admits;
    if (!admits.empty() && !listed(admits, want)) return &rule;
    if (listed(rule.refuses, want)) return &rule;
  }
  return nullptr;
}

auto describe(
  const Manifest &manifest, const String &axis, const String &want,
  const Entry &entry) -> String {
  const Rule *rule = explain(axis, want);
  if (!rule)
    return "Cartridge demands " + axis + " \"" + want +
           "\"; this host compiled " + CARTRIDGE::AXES::compiled(entry) + ".";
  const String head = manifest.name + " demands `" + axis + ": " + want +
                      "`; `" + rule->setting + ": " + rule->value + "` ";
  const String admits = rule->admits;
  if (!admits.empty() && !listed(admits, want))
    return head + "admits only `" + axis + ": " + admits + "` — refused";
  return head + "refuses `" + axis + ": " + want + "` — refused";
}
}  // namespace

static auto choose(
  const CARTRIDGE::Demand &demand, const CARTRIDGE::AXES::Entry &axis,
  LOGGER::Category &logger) -> Status {
  String offered;
  for (const auto &want : demand.values) {
    if (CARTRIDGE::AXES::carries(axis, CARTRIDGE::AXES::token(axis, want)))
      return 0;
    offered += offered.empty() ? want : ", " + want;
  }
  logger.error(
    "Cartridge demands " + demand.axis + " one of \"" + offered +
    "\"; this host compiled " + CARTRIDGE::AXES::compiled(axis) + ".");
  return 1;
}

auto CARTRIDGE::MANIFEST::vet(
  const Manifest &manifest, LOGGER::Category &logger) -> Status {
  Status status = 0;
  for (const auto &demand : manifest.demands) {
    const AXES::Entry *axis = AXES::find(demand.axis);
    if (!axis || !axis->demanded) continue;
    if (!axis->mask && demand.values.size() > 1) {
      status |= choose(demand, *axis, logger);
      continue;
    }
    for (const auto &want : demand.values) {
      const Whole wanted = AXES::token(*axis, want);
      if (!wanted) {
        logger.error(
          "Cartridge demands unknown " + demand.axis + " value \"" + want +
          "\".");
        status = 1;
      } else if (!AXES::carries(*axis, wanted)) {
        logger.error(describe(manifest, demand.axis, want, *axis));
        status = 1;
      }
    }
  }
  return status;
}
