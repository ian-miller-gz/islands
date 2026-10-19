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
#include <common/platform/paths.hpp>

#include <cstdlib>
#include <filesystem>
#include <string>

#define NOMINMAX
#include <windows.h>

namespace COMMON::PLATFORM {
namespace {
using Length = DWORD;
constexpr Length LIMIT = 32768;
constexpr Length REFUSED = 0;
constexpr STRING::Hot LOCAL = "LOCALAPPDATA";
constexpr STRING::Hot SUFFIX = ".exe";

namespace VARIABLE {
constexpr STRING::Hot STATE = "XDG_STATE_HOME";
constexpr STRING::Hot DATA = "XDG_DATA_HOME";
}  // namespace VARIABLE

auto rooted(STRING::Hot named) -> String {
  const Char *value = std::getenv(named);
  if (value && *value) return value;
  value = std::getenv(LOCAL);
  if (value && *value) return value;
  return String();
}
}  // namespace
}  // namespace COMMON::PLATFORM

auto COMMON::PLATFORM::executable() -> String {
  std::wstring buffer(LIMIT, L'\0');
  const Length length = GetModuleFileNameW(nullptr, buffer.data(), LIMIT);
  if (length == REFUSED || length == LIMIT) return String();
  buffer.resize(length);
  return std::filesystem::path(buffer).parent_path().string();
}

auto COMMON::PLATFORM::identity() -> Whole {
  return static_cast<Whole>(GetCurrentProcessId());
}

auto COMMON::PLATFORM::state() -> String { return rooted(VARIABLE::STATE); }

auto COMMON::PLATFORM::data() -> String { return rooted(VARIABLE::DATA); }

auto COMMON::PLATFORM::binary(const String &name) -> String {
  return name + SUFFIX;
}

auto COMMON::PLATFORM::assign(const String &name, const String &value)
  -> Status {
  return _putenv_s(name.c_str(), value.c_str());
}
