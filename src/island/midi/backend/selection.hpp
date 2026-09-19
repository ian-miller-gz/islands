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

#include <generated/values.hpp>
#include <generated/midi.hpp>
#include <common/platform/selection.hpp>

#if !defined(SR_MIDI_BACKEND) || !defined(SR_NONE) || !defined(SR_ALSA)
#error \
  "SR_MIDI_BACKEND/SR_NONE/SR_ALSA are not defined; set them in configs/make.yaml (tokens:)."
#elif SR_MIDI_BACKEND != SR_NONE && SR_MIDI_BACKEND != SR_ALSA
#error "SR_MIDI_BACKEND has an unknown value; expected SR_NONE or SR_ALSA."
#endif

#if SR_PLATFORM == SR_WEB && SR_MIDI_BACKEND != SR_NONE
#error \
  "SR_PLATFORM=SR_WEB (the browser target) requires SR_MIDI_BACKEND=SR_NONE."
#endif

#if SR_PLATFORM == SR_WINDOWS && SR_MIDI_BACKEND != SR_NONE
#error \
  "SR_PLATFORM=SR_WINDOWS (the Windows target) requires SR_MIDI_BACKEND=SR_NONE."
#endif
