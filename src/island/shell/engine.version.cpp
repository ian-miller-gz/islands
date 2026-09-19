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
#include <cartridge/selection.hpp>
#include <island/graphics/backend/selection.hpp>
#include <island/overlay/backend/selection.hpp>
#include <island/shell/engine.internal.hpp>
#include <island/terminal/backend/selection.hpp>
#include <island/window/backend/selection.hpp>
#include <format>
#include <generated/profile.hpp>
#include <logger/backend/selection.hpp>
#include <network/backend/selection.hpp>

#if !defined(SR_PROFILE)
#error "SR_PROFILE is not defined; set it in configs/make.yaml (tokens:)."
#endif

namespace AXIS {
constexpr STRING::Hot GFX =
  SR_GFX_BACKEND == SR_VULKAN ? "SR_VULKAN" : "SR_SDL3";
constexpr STRING::Hot WINDOW =
  SR_WINDOW_BACKEND == SR_GLFW ? "SR_GLFW" : "SR_SDL3";
constexpr STRING::Hot OVERLAY =
  SR_OVERLAY_BACKEND == SR_IMGUI ? "SR_IMGUI" : "SR_NONE";
constexpr STRING::Hot LOGGER =
  SR_LOGGER_BACKEND == SR_LOG4CPP ? "SR_LOG4CPP" : "SR_SPDLOG";
constexpr STRING::Hot NETWORK = SR_NETWORK_BACKEND == SR_UNIX ? "SR_UNIX"
                                : SR_NETWORK_BACKEND == SR_ABSTRACT
                                  ? "SR_ABSTRACT"
                                : SR_NETWORK_BACKEND == SR_TCP  ? "SR_TCP"
                                : SR_NETWORK_BACKEND == SR_TCP6 ? "SR_TCP6"
                                : SR_NETWORK_BACKEND == SR_SCTP ? "SR_SCTP"
                                                                : "unknown";
constexpr STRING::Hot DATAGRAM =
  SR_DATAGRAM_BACKEND == SR_UDP ? "SR_UDP" : "SR_NONE";
constexpr STRING::Hot LISTEN = SR_LISTEN == SR_UNIX ? "SR_UNIX" : "SR_NONE";
constexpr STRING::Hot PROFILE =
  SR_PROFILE == SR_DEBUG ? "SR_DEBUG" : "SR_RELEASE";

constexpr STRING::Hot DELIVERY = SR_CARTRIDGE == SR_NONE ? "dynamic loader"
                                 : SR_CARTRIDGE == SR_MEADOW
                                   ? "static: SR_MEADOW"
                                   : "static: unknown";
}  // namespace AXIS

static constexpr Whole WIDTH = 8;

void ISLAND::SHELL::version(::SHELL::Session &session) {
  session.print(std::format("{:{}} | {}", "gfx", WIDTH, AXIS::GFX));
  session.print(std::format("{:{}} | {}", "window", WIDTH, AXIS::WINDOW));
  session.print(std::format("{:{}} | {}", "overlay", WIDTH, AXIS::OVERLAY));
  session.print(std::format("{:{}} | {}", "logger", WIDTH, AXIS::LOGGER));
  session.print(std::format("{:{}} | {}", "network", WIDTH, AXIS::NETWORK));
  session.print(std::format("{:{}} | {}", "datagram", WIDTH, AXIS::DATAGRAM));
  session.print(std::format("{:{}} | {}", "listen", WIDTH, AXIS::LISTEN));
  session.print(std::format("{:{}} | {}", "profile", WIDTH, AXIS::PROFILE));
  session.print(std::format("{:{}} | {}", "delivery", WIDTH, AXIS::DELIVERY));
}
