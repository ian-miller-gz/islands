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
#include <island/window/notice.internal.hpp>

namespace WINDOW::NOTICE::GIO {
using Truth = int;
using Number = unsigned int;
using Value = void;
using Bus = void;

constexpr Truth NO = 0;
constexpr int PLAIN = 0;
constexpr int WAIT = 1000;
constexpr STRING::Hot NAME = "org.freedesktop.Notifications";
constexpr STRING::Hot PATH = "/org/freedesktop/Notifications";
constexpr STRING::Hot INTERFACE = NAME;
constexpr STRING::Hot ACTION = "default";

using Handler = void (*)(
  Bus *, const char *, const char *, const char *, const char *, Value *,
  void *);

struct Slice {
  Bus *(*get)(int, void *, void *) = nullptr;
  void (*keep)(Bus *, const char *, ...) = nullptr;
  Value *(*make)(const char *, ...) = nullptr;
  void (*read)(Value *, const char *, ...) = nullptr;
  void (*drop)(Value *) = nullptr;
  Value *(*call)(
    Bus *, const char *, const char *, const char *, const char *, Value *,
    const void *, int, int, void *, void *) = nullptr;
  void (*send)(
    Bus *, const char *, const char *, const char *, const char *, Value *,
    const void *, int, int, void *, void *, void *) = nullptr;
  Truth (*closed)(Bus *) = nullptr;
  Number (*subscribe)(
    Bus *, const char *, const char *, const char *, const char *, const char *,
    int, Handler, void *, void *) = nullptr;
  Truth (*turn)(void *, Truth) = nullptr;
};

auto slice() -> const Slice *;

auto connect(const Slice &gio) -> Bus *;
}  // namespace WINDOW::NOTICE::GIO

namespace WINDOW::NOTICE::GIO::STATE {
extern Bus *bus;
}  // namespace WINDOW::NOTICE::GIO::STATE
