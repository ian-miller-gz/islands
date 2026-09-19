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

#include <common/types.hpp>

namespace AUDIO::PLUGIN {

using Sample = Float;

struct Event {
  enum Kind : Whole { NOTE_ON, NOTE_OFF, CONTROLLER, BEND, TRANSPORT, PROGRAM };
  Whole kind = NOTE_ON;
  Whole offset = 0;
  Whole index = 0;
  Float value = 0;
};

struct Control {
  String unit;
  Whole steps = 0;
  Vector<String> labels;
  Float resting = 0;
  Float least = 0;
  Float most = 0;
};

struct Port {
  enum Kind : Whole { AUDIO, NOTES, CONTROL, PROGRAM, LOGIC, DATA };
  Whole kind = AUDIO;
  String name;
  String type;
};

struct Plug {
  auto (*create)(Whole rate, Whole channels) -> void *;

  void (*render)(
    void *instance, Sample *const *lanes, Whole frames, const Event *events,
    Whole count);

  auto (*meter)(void *instance) -> Float;

  void (*destroy)(void *instance);

  auto (*parameters)(void *instance) -> Whole;

  auto (*name)(void *instance, Whole index) -> String;

  auto (*reading)(void *instance, Whole index) -> String;

  auto (*held)(void *instance, Whole index) -> Float;

  auto (*control)(void *instance, Whole index, Control &out) -> Flag;

  Vector<Port> ins;
  Vector<Port> outs;
};

}  // namespace AUDIO::PLUGIN
