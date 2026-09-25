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
#include <cartridge/axes.hpp>
#include <cartridge/axes.internal.hpp>
#include <common/platform/selection.hpp>
#include <island/graphics/backend/selection.hpp>
#include <island/gui/backend/selection.hpp>
#include <island/gui/sac.selection.hpp>
#include <island/midi/backend/selection.hpp>
#include <island/overlay/backend/selection.hpp>
#include <island/terminal/backend/selection.hpp>
#include <island/window/backend/selection.hpp>
#include <logger/backend/selection.hpp>
#include <metrics/selection.hpp>
#include <network/selection.hpp>
#include <network/web/backend/selection.hpp>
#include <network/web/selection.hpp>
#include <topics/selection.hpp>

#include <generated/profile.hpp>

namespace {
using CARTRIDGE::AXES::Entry;
using CARTRIDGE::AXES::Value;

constexpr Value PLATFORM[] = {
  {"posix", SR_POSIX}, {"web", SR_WEB}, {"windows", SR_WINDOWS}};
constexpr Value GFX[] = {{"vulkan", SR_VULKAN}, {"sdl3", SR_SDL3}};
constexpr Value WINDOW[] = {{"glfw", SR_GLFW}, {"sdl3", SR_SDL3}};
constexpr Value OVERLAY[] = {{"imgui", SR_IMGUI}, {"none", SR_NONE}};
constexpr Value GUI[] = {{"native", SR_NATIVE}, {"rmlui", SR_RMLUI}};
constexpr Value SAC[] = {{"native", SR_NATIVE}, {"none", SR_NONE}};
constexpr Value LOGGER[] = {{"log4cpp", SR_LOG4CPP}, {"spdlog", SR_SPDLOG}};
constexpr Value NETWORK[] = {{"unix", SR_UNIX}, {"abstract", SR_ABSTRACT},
                             {"tcp", SR_TCP},   {"tcp6", SR_TCP6},
                             {"sctp", SR_SCTP}, {"vsock", SR_VSOCK},
                             {"tls", SR_TLS},   {"none", SR_NONE}};
constexpr Value DATAGRAM[] = {{"udp", SR_UDP}, {"none", SR_NONE}};
constexpr Value LISTEN[] = {{"unix", SR_UNIX}, {"none", SR_NONE}};
constexpr Value MIDI[] = {{"alsa", SR_ALSA}, {"none", SR_NONE}};
constexpr Value WEB[] = {{"none", SR_NONE}, {"native", SR_NATIVE}};
constexpr Value SERVER[] = {{"none", SR_NONE}, {"native", SR_NATIVE}};
constexpr Value TOPICS[] = {{"local", SR_LOCAL}, {"none", SR_NONE}};
constexpr Value METRICS[] = {{"registry", SR_REGISTRY}, {"none", SR_NONE}};
constexpr Value PROFILE[] = {{"debug", SR_DEBUG}, {"release", SR_RELEASE}};

template <Whole COUNT>
constexpr auto counted(const Value (&)[COUNT]) -> Whole {
  return COUNT;
}

constexpr Entry TABLE[] = {
  {CARTRIDGE::AXES::PLATFORM_AXIS, PLATFORM, counted(PLATFORM), SR_PLATFORM, 0,
   true, false},
  {"gfx", GFX, counted(GFX), SR_GFX_BACKEND, 0, true, true},
  {"window", WINDOW, counted(WINDOW), SR_WINDOW_BACKEND, 0, true, true},
  {"overlay", OVERLAY, counted(OVERLAY), SR_OVERLAY_BACKEND, 0, true, true},
  {"gui", GUI, counted(GUI), SR_GUI_BACKEND, 0, true, true},
  {"sac", SAC, counted(SAC), SR_SAC, 0, true, true},
  {"logger", LOGGER, counted(LOGGER), SR_LOGGER_BACKEND, 0, false, false},
  {"network", NETWORK, counted(NETWORK), SR_NETWORK_BACKEND,
   SR_NETWORK_BACKEND_SET, true, false},
  {"datagram", DATAGRAM, counted(DATAGRAM), SR_DATAGRAM_BACKEND, 0, true,
   false},
  {"listen", LISTEN, counted(LISTEN), SR_LISTEN, 0, true, false},
  {"midi", MIDI, counted(MIDI), SR_MIDI_BACKEND, 0, true, true},
  {"web", WEB, counted(WEB), SR_WEB_BACKEND, 0, false, false},
  {"server", SERVER, counted(SERVER), SR_SERVER, 0, false, false},
  {"topics", TOPICS, counted(TOPICS), SR_TOPICS, 0, false, false},
  {"metrics", METRICS, counted(METRICS), SR_METRICS, 0, false, false},
  {"profile", PROFILE, counted(PROFILE), SR_PROFILE, 0, false, false}};
}  // namespace

auto CARTRIDGE::AXES::find(const String &name) -> const Entry * {
  for (const auto &entry : TABLE)
    if (name == entry.name) return &entry;
  return nullptr;
}

auto CARTRIDGE::AXES::platform() -> String {
  const Entry *entry = find(PLATFORM_AXIS);
  return entry ? compiled(*entry) : String();
}

auto CARTRIDGE::GET::axes() -> Vector<Axis> {
  Vector<Axis> report;
  for (const auto &entry : TABLE) {
    Axis axis{
      .name = entry.name,
      .demanded = entry.demanded,
      .graphical = entry.graphical};
    for (Whole at = 0; at < entry.count; at += 1) {
      axis.values.push_back(entry.values[at].spelling);
      if (AXES::carries(entry, entry.values[at].token))
        axis.carried.push_back(entry.values[at].spelling);
    }
    report.push_back(axis);
  }
  return report;
}
