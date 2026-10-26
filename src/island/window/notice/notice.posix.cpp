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
#include <island/window/notice.posix.internal.hpp>

namespace WINDOW::NOTICE::GIO {
namespace {

constexpr STRING::Hot NOTIFY = "Notify";
constexpr STRING::Hot CLOSE = "CloseNotification";
constexpr STRING::Hot SHAPE = "(susss^asa{sv}i)";
constexpr STRING::Hot NUMBER = "(u)";
constexpr STRING::Hot BLANK = "";
constexpr int EXPIRY = -1;

constexpr STRING::Hot ACTIONS[] = {ACTION, BLANK, nullptr};

auto notify(const Slice &gio, Bus *bus, Value *arguments) -> Value * {
  return gio.call(
    bus, NAME, PATH, INTERFACE, NOTIFY, arguments, nullptr, PLAIN, WAIT,
    nullptr, nullptr);
}

}  // namespace
}  // namespace WINDOW::NOTICE::GIO

auto WINDOW::NOTICE::show(STRING::Hot title, Id standing, Id &shown) -> Status {
  using namespace GIO;
  const Slice *gio = slice();
  if (gio == nullptr) return NOTIFIER;
  Bus *bus = connect(*gio);
  if (bus == nullptr) return BUS;
  Value *arguments = gio->make(
    SHAPE, BLANK, standing, BLANK, title, BLANK, ACTIONS, nullptr, EXPIRY);
  Value *answer = notify(*gio, bus, arguments);
  if (answer == nullptr) return NOTIFIER;
  gio->read(answer, NUMBER, &shown);
  gio->drop(answer);
  return SHOWN;
}

void WINDOW::NOTICE::close(Id id) {
  using namespace GIO;
  if (GIO::STATE::bus == nullptr) return;
  const Slice &gio = *slice();
  gio.send(
    GIO::STATE::bus, NAME, PATH, INTERFACE, CLOSE, gio.make(NUMBER, id),
    nullptr, PLAIN, WAIT, nullptr, nullptr, nullptr);
}

void WINDOW::NOTICE::poll() {
  using namespace GIO;
  if (GIO::STATE::bus != nullptr) slice()->turn(nullptr, NO);
}
