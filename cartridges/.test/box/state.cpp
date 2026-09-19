// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

GFX::MODEL::Scene BOX::scene;
GFX::Handle BOX::sampler = 0;
GFX::Pass BOX::pass;
GFX::Handle BOX::PIPELINES::unlit = 0;
GFX::Handle BOX::PIPELINES::pbr = 0;
