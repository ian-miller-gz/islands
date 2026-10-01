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
#include <string_view>

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

// A launch through a link (winget's Links folder, a shortcut's target
// kept as a symbolic link) names the link, and the home lies beside the
// file itself: the final path, the \\?\ prefix the kernel adds shed.
auto resolved(const std::wstring &path) -> std::wstring {
  const HANDLE file = CreateFileW(
    path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
    nullptr);
  if (file == INVALID_HANDLE_VALUE) return path;
  std::wstring buffer(LIMIT, L'\0');
  const Length length =
    GetFinalPathNameByHandleW(file, buffer.data(), LIMIT, FILE_NAME_NORMALIZED);
  CloseHandle(file);
  if (length == REFUSED || length >= LIMIT) return path;
  buffer.resize(length);
  constexpr std::wstring_view UNC = L"\\\\?\\UNC\\";
  constexpr std::wstring_view LOCAL_PREFIX = L"\\\\?\\";
  if (buffer.starts_with(UNC)) return L"\\\\" + buffer.substr(UNC.size());
  if (buffer.starts_with(LOCAL_PREFIX)) return buffer.substr(LOCAL_PREFIX.size());
  return buffer;
}
}  // namespace
}  // namespace COMMON::PLATFORM

auto COMMON::PLATFORM::executable() -> String {
  std::wstring buffer(LIMIT, L'\0');
  const Length length = GetModuleFileNameW(nullptr, buffer.data(), LIMIT);
  if (length == REFUSED || length == LIMIT) return String();
  buffer.resize(length);
  return std::filesystem::path(resolved(buffer)).parent_path().string();
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
