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

#include <climits>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>

namespace COMMON::PLATFORM {
namespace {
constexpr STRING::Hot SELF = "/proc/self/exe";
constexpr STRING::Hot ACCOUNT = "HOME";
constexpr Status OVERWRITE = 1;

namespace VARIABLE {
constexpr STRING::Hot STATE = "XDG_STATE_HOME";
constexpr STRING::Hot DATA = "XDG_DATA_HOME";
}  // namespace VARIABLE

namespace FALLBACK {
constexpr STRING::Hot STATE = ".local/state";
constexpr STRING::Hot DATA = ".local/share";
}  // namespace FALLBACK

auto rooted(STRING::Hot named, STRING::Hot beneath) -> String {
  const Char *value = std::getenv(named);
  if (value && *value) return value;
  const Char *account = std::getenv(ACCOUNT);
  if (!account || !*account) return String();
  return (std::filesystem::path(account) / beneath).string();
}
}  // namespace
}  // namespace COMMON::PLATFORM

auto COMMON::PLATFORM::executable() -> String {
  Char buffer[PATH_MAX];
  const ssize_t length = readlink(SELF, buffer, sizeof(buffer) - 1);
  if (length == -1) return String();
  buffer[length] = '\0';
  return std::filesystem::path(buffer).parent_path().string();
}

auto COMMON::PLATFORM::identity() -> Whole {
  return static_cast<Whole>(getpid());
}

auto COMMON::PLATFORM::state() -> String {
  return rooted(VARIABLE::STATE, FALLBACK::STATE);
}

auto COMMON::PLATFORM::data() -> String {
  return rooted(VARIABLE::DATA, FALLBACK::DATA);
}

auto COMMON::PLATFORM::binary(const String &name) -> String { return name; }

auto COMMON::PLATFORM::assign(const String &name, const String &value)
  -> Status {
  return setenv(name.c_str(), value.c_str(), OVERWRITE);
}
