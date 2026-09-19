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
#include <island/gui/gui.internal.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/windows.hpp>

auto GUI::mount(STRING::Hot assets, STRING::Hot layout) -> Handle {
  return load((String(assets) + layout).c_str());
}

auto GUI::frame(Handle document, GFX::Color clear) -> Status {
  const GFX::Viewport viewport = GFX::WINDOWS::MAIN::viewport();
  const Status placed = place(document, viewport);
  poll();
  GFX::Pass pass = {.clear = clear, .viewport = viewport};
  flush(document, pass);
  GFX::PASSES::submit(std::move(pass));
  return placed;
}
