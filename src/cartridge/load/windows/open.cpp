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
#include <filesystem>
#include <string>

#define NOMINMAX
#define NOUSER
#include <windows.h>

namespace CARTRIDGE {
namespace {

using Entry = auto (*)() -> const Interface &;
using Wide = std::wstring;
constexpr STRING::Hot SYMBOL = "cartridge";
constexpr STRING::Hot UNKNOWN = "The module refused without a reason.";
constexpr DWORD SYSTEM = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                         FORMAT_MESSAGE_FROM_SYSTEM |
                         FORMAT_MESSAGE_IGNORE_INSERTS;
constexpr DWORD ANY = 0;

HMODULE module = nullptr;

auto refusal() -> String {
  wchar_t *text = nullptr;
  const DWORD length = FormatMessageW(
    SYSTEM, nullptr, GetLastError(), ANY, reinterpret_cast<wchar_t *>(&text), 0,
    nullptr);
  if (!length) return String(UNKNOWN);
  Wide wide(text, length);
  LocalFree(text);
  while (!wide.empty() && (wide.back() == L'\n' || wide.back() == L'\r'))
    wide.pop_back();
  return std::filesystem::path(wide).string();
}

}  // namespace
}  // namespace CARTRIDGE

auto CARTRIDGE::open(const String &file, LOGGER::Category &logger)
  -> const Interface * {
  module = LoadLibraryW(std::filesystem::path(file).wstring().c_str());
  auto symbol = module ? GetProcAddress(module, SYMBOL) : nullptr;
  if (!symbol) {
    logger.error(file + ": " + refusal());
    return nullptr;
  }
  return &reinterpret_cast<Entry>(symbol)();
}

void CARTRIDGE::release() {
  if (!module) return;
  FreeLibrary(module);
  module = nullptr;
}

#endif
