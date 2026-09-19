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
#pragma once

#include <cartridge.hpp>
#include <network.hpp>
#include <metrics.hpp>
#include <topics.hpp>
#include <island/audio.hpp>
#include <island/input.hpp>
#include <island/gui.hpp>
#include <island/midi.hpp>
#include <island/overlay.hpp>
#include <island/graphics.hpp>
#include <island/process.hpp>
#include <island/terminal.hpp>
#include <common.hpp>

namespace ISLAND {
static inline void close();
}

static inline void ISLAND::close() {
  join();
  CARTRIDGE::close();
  INPUT::POINTER::capture(false);
  INPUT::TEXT::focus(false);
  AUDIO::OUTPUT::remove();
  AUDIO::INPUT::remove();
  AUDIO::stop();
  MIDI::close();
  GUI::close();
  GUI::DIALOG::close();
  TOPICS::reset();
  METRICS::reset();
  NETWORK::INSTANCES::close();
  NETWORK::WEB::SERVER::close();
  OVERLAY::close();
  GFX::close();
  TERMINAL::close();
  REQUIREMENTS::close();
  RELATIONS::close();
  CARTRIDGE::unload();
}