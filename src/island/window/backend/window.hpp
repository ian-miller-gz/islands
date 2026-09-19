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

#include <common.hpp>
#include <island/window/backend/selection.hpp>
#include <island/graphics/backend/selection.hpp>

namespace WINDOW::BACKEND::FALLBACK {
constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;
}  // namespace WINDOW::BACKEND::FALLBACK

namespace WINDOW::BACKEND::ORIGIN {
constexpr int X = 100;
constexpr int Y = 100;
}  // namespace WINDOW::BACKEND::ORIGIN

namespace WINDOW::BACKEND {
using Native = unsigned long long;
constexpr Native NONE = 0;
}  // namespace WINDOW::BACKEND

namespace WINDOW::BACKEND {
extern Flag visible;

extern Flag decorated;

void initialize();
void create(STRING::Hot title, int width, int height);
Flag poll();
void destroy();

void close();

void minimize();

void drag(Flag on);

void maximize(Flag on);

void fullscreen(Flag on);

void capture(Flag on);

void clipboard(STRING::Hot text);

enum Shape { ARROW, IBEAM };

struct Image {
  Whole w = 0;
  Whole h = 0;
  Vector<Byte> pixels;
};

constexpr Whole CHANNELS = 4;

void wear(const Image &image, Whole hotx, Whole hoty);

void shape(Shape wanted);
}  // namespace WINDOW::BACKEND

namespace WINDOW::BACKEND::GET {
void size(int *width, int *height);
Float scale();
Flag live();
Flag fullscreen();
Flag worn();
Flag maximized();
String clipboard();

Native parent();
}  // namespace WINDOW::BACKEND::GET

#if SR_GFX_BACKEND == SR_VULKAN
#include <vulkan/vulkan.h>
namespace WINDOW::BACKEND::SURFACE {
VkSurfaceKHR create(VkInstance instance);
}  // namespace WINDOW::BACKEND::SURFACE
namespace WINDOW::BACKEND::GET {
Vector<STRING::Hot> extensions();
}  // namespace WINDOW::BACKEND::GET
#endif

#if SR_GFX_BACKEND == SR_SDL3
struct SDL_Window;
namespace WINDOW::BACKEND::GET {
SDL_Window *handle();
}  // namespace WINDOW::BACKEND::GET
#endif

namespace WINDOW {
using namespace ::WINDOW::BACKEND;
}  // namespace WINDOW
