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
#define NOMINMAX
#include <windows.h>

#include <new>
#include <string>

#include <conduit/conduit.internal.hpp>
#include <logger.hpp>

namespace {
static const String category = "~/conduit::";

constexpr uint64_t HALF = 32;

auto identifier(const String &name) -> String {
  return "Local\\" + CONDUIT::leaf(name);
}

auto attach(const String &object, Whole bytes, Flag make) -> CONDUIT::Segment {
  const std::wstring wide(object.begin(), object.end());
  const uint64_t span = bytes;
  HANDLE section =
    make ? CreateFileMappingW(
             INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
             static_cast<DWORD>(span >> HALF), static_cast<DWORD>(span),
             wide.c_str())
         : OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, wide.c_str());
  if (section == nullptr) return {};
  void *base = MapViewOfFile(
    section, FILE_MAP_ALL_ACCESS, 0, 0, static_cast<SIZE_T>(bytes));
  if (base == nullptr) {
    CloseHandle(section);
    return {};
  }
  return {.name = object, .base = base, .bytes = bytes, .mapping = section};
}

}  // namespace

auto CONDUIT::create(const String &name, Whole records, Whole stride)
  -> Handle {
  const Whole slots = rounded(records);
  const Whole bytes = HEADER + slots * stride;
  Segment segment = attach(identifier(name), bytes, true);
  if (segment.base == nullptr) {
    LOGGER::get(category + "create")
      .debug("section '%s' refused", name.c_str());
    return NONE;
  }
  Control *control = new (segment.base) Control{};
  control->records = slots;
  control->stride = stride;
  segment.owner = true;
  return seat(segment);
}

auto CONDUIT::open(const String &name, Whole records, Whole stride) -> Handle {
  const Whole slots = rounded(records);
  const Whole bytes = HEADER + slots * stride;
  Segment segment = attach(identifier(name), bytes, false);
  if (segment.base == nullptr) return NONE;
  auto *control = static_cast<Control *>(segment.base);
  if (control->records != slots || control->stride != stride) {
    UnmapViewOfFile(segment.base);
    CloseHandle(segment.mapping);
    LOGGER::get(category + "open").debug("section '%s' geometry", name.c_str());
    return NONE;
  }
  return seat(segment);
}

void CONDUIT::destroy(Handle conduit) {
  Segment *segment = behind(conduit);
  if (segment == nullptr) return;
  UnmapViewOfFile(segment->base);
  CloseHandle(segment->mapping);
  segment->base = nullptr;
  segment->mapping = nullptr;
  segment->control = nullptr;
  segment->slots = nullptr;
}
