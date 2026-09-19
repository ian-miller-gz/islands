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
#include <island/graphics/sprites/sprites.internal.hpp>
#include <logger.hpp>
#include <algorithm>
#include <string>
#define LOGGER_CATEGORY "~/island/graphics/sprites::"

namespace STATE = GFX::SPRITES::STATE;
namespace ATLASES = GFX::SPRITES::ATLASES;

constexpr STRING::Hot DESCRIBED = ".atlas";

static auto find(GFX::SPRITES::Handle atlas, STRING::Hot name)
  -> const GFX::SPRITES::Named * {
  for (const auto &frame : STATE::atlases[atlas].frames)
    if (frame.name == name) return &frame;
  LOGGER::get(LOGGER_CATEGORY "atlases")
    .error(String("No frame named '") + name + "' on this atlas.");
  return nullptr;
}

auto ATLASES::create(STRING::Hot path) -> Handle {
  String extension(path);
  const auto dot = extension.find_last_of('.');
  extension = dot == String::npos ? String() : extension.substr(dot);
  std::transform(
    extension.begin(), extension.end(), extension.begin(),
    [](Char letter) { return static_cast<Char>(std::tolower(letter)); });
  if (extension == DESCRIBED) return GFX::SPRITES::descriptor(path);
  return create(path, {});
}

auto ATLASES::frame(Handle atlas, STRING::Hot name) -> Frame {
  const auto *found = ::find(atlas, name);
  return found ? found->placement.texture : Frame{};
}

auto ATLASES::GET::names(Handle atlas) -> Vector<String> {
  Vector<String> answer;
  for (const auto &frame : STATE::atlases[atlas].frames)
    answer.push_back(frame.name);
  return answer;
}

auto ATLASES::GET::placement(Handle atlas, STRING::Hot name) -> Placement {
  const auto *found = ::find(atlas, name);
  return found ? found->placement : Placement{};
}
