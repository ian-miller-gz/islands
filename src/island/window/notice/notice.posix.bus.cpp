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

#include <dlfcn.h>

WINDOW::NOTICE::GIO::Bus *WINDOW::NOTICE::GIO::STATE::bus = nullptr;

namespace WINDOW::NOTICE::GIO {
namespace {

constexpr STRING::Hot LIBRARY = "libgio-2.0.so.0";
constexpr STRING::Hot INVOKED = "ActionInvoked";
constexpr STRING::Hot CLOSED = "NotificationClosed";
constexpr STRING::Hot CLICK = "(u&s)";
constexpr STRING::Hot END = "(uu)";
constexpr STRING::Hot EXIT = "exit-on-close";
constexpr int SESSION = 2;

template <typename Entry>
auto bind(void *library, STRING::Hot symbol, Entry &into) -> Flag {
  into = reinterpret_cast<Entry>(dlsym(library, symbol));
  return into != nullptr;
}

auto open(Slice &gio) -> Flag {
  void *library = dlopen(LIBRARY, RTLD_NOW | RTLD_LOCAL);
  if (library == nullptr) return false;
  return bind(library, "g_bus_get_sync", gio.get) &&
         bind(library, "g_object_set", gio.keep) &&
         bind(library, "g_variant_new", gio.make) &&
         bind(library, "g_variant_get", gio.read) &&
         bind(library, "g_variant_unref", gio.drop) &&
         bind(library, "g_dbus_connection_call_sync", gio.call) &&
         bind(library, "g_dbus_connection_call", gio.send) &&
         bind(library, "g_dbus_connection_is_closed", gio.closed) &&
         bind(library, "g_dbus_connection_signal_subscribe", gio.subscribe) &&
         bind(library, "g_main_context_iteration", gio.turn);
}

void clicked(
  Bus *, const char *, const char *, const char *, const char *, Value *values,
  void *) {
  Id id = NONE;
  const char *action = nullptr;
  slice()->read(values, CLICK, &id, &action);
  if (id == NONE || id != NOTICE::STATE::standing) return;
  if (action != nullptr && std::string_view{action} == ACTION) WINDOW::raise();
}

void ended(
  Bus *, const char *, const char *, const char *, const char *, Value *values,
  void *) {
  Id id = NONE;
  Number reason = 0;
  slice()->read(values, END, &id, &reason);
  if (id == NOTICE::STATE::standing) NOTICE::STATE::standing = NONE;
}

void listen(const Slice &gio, Bus *bus, STRING::Hot signal, Handler handler) {
  gio.subscribe(
    bus, NAME, INTERFACE, signal, PATH, nullptr, PLAIN, handler, nullptr,
    nullptr);
}

}  // namespace
}  // namespace WINDOW::NOTICE::GIO

auto WINDOW::NOTICE::GIO::slice() -> const Slice * {
  static Slice gio;
  static const Flag opened = open(gio);
  return opened ? &gio : nullptr;
}

auto WINDOW::NOTICE::GIO::connect(const Slice &gio) -> Bus * {
  if (STATE::bus != nullptr)
    return gio.closed(STATE::bus) ? nullptr : STATE::bus;
  Bus *bus = gio.get(SESSION, nullptr, nullptr);
  if (bus == nullptr) return nullptr;
  gio.keep(bus, EXIT, NO, nullptr);
  listen(gio, bus, INVOKED, clicked);
  listen(gio, bus, CLOSED, ended);
  return STATE::bus = bus;
}
