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
#include <cstdint>

#include <threads.hpp>
#include "reed.internal.hpp"

namespace {

struct Saw {
  int32_t phase = 0;
  int32_t period = 1;
  Flag gated = false;
  Float gain = 0;
  Float level[2] = {0, 0};
  THREADS::Shared<Whole> face{0};
};

void apply(Saw &saw, const AUDIO::PLUGIN::Event &event) {
  switch (event.kind) {
    case AUDIO::PLUGIN::Event::NOTE_ON:
      saw.period = event.index < 1 ? 1 : static_cast<int32_t>(event.index);
      saw.gain = event.value;
      saw.phase = 0;
      saw.gated = true;
      break;
    case AUDIO::PLUGIN::Event::NOTE_OFF:
      saw.gated = false;
      break;
    case AUDIO::PLUGIN::Event::CONTROLLER:
      if (event.index == 0)
        saw.gain = event.value < 0 ? 0 : event.value > 1 ? 1 : event.value;
      break;
    default:
      break;
  }
}

void publish(Saw &saw, Float peak) {
  const Whole idle = saw.face.load(std::memory_order_relaxed) ^ 1u;
  saw.level[idle] = peak;
  saw.face.store(idle, std::memory_order_release);
}

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  return new Saw{};
}

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *outputs, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  auto &saw = *static_cast<Saw *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(saw, events[next++]);
    Float sample = 0;
    if (saw.gated) {
      const Float turn = static_cast<Float>(saw.phase % saw.period);
      sample = saw.gain * (2.0f * turn / static_cast<Float>(saw.period) - 1.0f);
      saw.phase = (saw.phase + 1) % saw.period;
    }
    outputs[0][frame] = sample;
    const Float magnitude = sample < 0 ? -sample : sample;
    if (magnitude > peak) peak = magnitude;
  }
  while (next < count) apply(saw, events[next++]);
  publish(saw, peak);
}

auto meter(void *instance) -> Float {
  auto &saw = *static_cast<Saw *>(instance);
  return saw.level[saw.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) { delete static_cast<Saw *>(instance); }

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = render,
  .meter = meter,
  .destroy = destroy,
  .ins = {{AUDIO::PLUGIN::Port::NOTES}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};
}  // namespace

auto REED::instrument() -> const AUDIO::PLUGIN::Plug & { return surface; }
