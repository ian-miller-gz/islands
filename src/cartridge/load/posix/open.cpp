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
#include <cartridge/selection.hpp>
#if SR_CARTRIDGE == SR_NONE
#include <cartridge/load.internal.hpp>
#include <dlfcn.h>

namespace CARTRIDGE {
namespace {

using Entry = auto (*)() -> const Interface &;
constexpr STRING::Hot SYMBOL = "cartridge";

void *module = nullptr;

}  // namespace
}  // namespace CARTRIDGE

auto CARTRIDGE::open(const String &file, LOGGER::Category &logger)
  -> const Interface * {
  module = dlopen(file.c_str(), RTLD_NOW | RTLD_LOCAL);
  auto symbol = module ? dlsym(module, SYMBOL) : nullptr;
  if (!symbol) {
    logger.error(dlerror());
    return nullptr;
  }
  return &reinterpret_cast<Entry>(symbol)();
}

void CARTRIDGE::release() {
  if (!module) return;
  dlclose(module);
  module = nullptr;
}

#endif
