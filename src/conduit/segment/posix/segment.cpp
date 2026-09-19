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
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <new>

#include <conduit/conduit.internal.hpp>
#include <logger.hpp>

namespace {
static const String category = "~/conduit::";

auto identifier(const String &name) -> String {
  return "/" + CONDUIT::leaf(name);
}

auto attach(const String &shm, Whole bytes, Flag make) -> void * {
  const int flags = make ? (O_CREAT | O_RDWR) : O_RDWR;
  const int fd = shm_open(shm.c_str(), flags, 0600);
  if (fd < 0) return nullptr;
  if (make && ftruncate(fd, static_cast<off_t>(bytes)) != 0) {
    ::close(fd);
    return nullptr;
  }
  void *base = mmap(nullptr, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  ::close(fd);
  return base == MAP_FAILED ? nullptr : base;
}

}  // namespace

auto CONDUIT::create(const String &name, Whole records, Whole stride)
  -> Handle {
  const Whole slots = rounded(records);
  const Whole bytes = HEADER + slots * stride;
  const String shm = identifier(name);
  shm_unlink(shm.c_str());
  void *base = attach(shm, bytes, true);
  if (base == nullptr) {
    LOGGER::get(category + "create").debug("shm '%s' refused", name.c_str());
    return NONE;
  }
  Control *control = new (base) Control{};
  control->records = slots;
  control->stride = stride;
  return seat({.name = shm, .owner = true, .base = base, .bytes = bytes});
}

auto CONDUIT::open(const String &name, Whole records, Whole stride) -> Handle {
  const Whole slots = rounded(records);
  const Whole bytes = HEADER + slots * stride;
  const String shm = identifier(name);
  void *base = attach(shm, bytes, false);
  if (base == nullptr) return NONE;
  auto *control = static_cast<Control *>(base);
  if (control->records != slots || control->stride != stride) {
    munmap(base, bytes);
    LOGGER::get(category + "open").debug("shm '%s' geometry", name.c_str());
    return NONE;
  }
  return seat({.name = shm, .owner = false, .base = base, .bytes = bytes});
}

void CONDUIT::destroy(Handle conduit) {
  Segment *segment = behind(conduit);
  if (segment == nullptr) return;
  munmap(segment->base, segment->bytes);
  if (segment->owner) shm_unlink(segment->name.c_str());
  segment->base = nullptr;
  segment->control = nullptr;
  segment->slots = nullptr;
}
