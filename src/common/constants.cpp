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
#include <generated/engine.hpp>
#include <common/types.hpp>
#include <common/constants.hpp>
#include <common/platform/paths.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>

static constexpr STRING::Hot NAME = "islands";
static constexpr STRING::Hot LIBEXEC = "libexec";
static constexpr STRING::Hot SHARE = "share";
static constexpr STRING::Hot PROBE = ".islands.probe.";

static String home(const String &directory) {
  const std::filesystem::path path(directory);
  if (!path.has_parent_path()) return directory;
  const std::filesystem::path parent = path.parent_path();
  if (path.filename().string() == BUILD_NAME) return parent.string();
  const Flag libexec = path.filename().string() == NAME &&
                       parent.has_parent_path() &&
                       parent.filename().string() == LIBEXEC;
  if (libexec) return (parent.parent_path() / SHARE / NAME).string();
  return directory;
}

static Flag writable(const std::filesystem::path &directory) {
  const std::filesystem::path probe =
    directory / (PROBE + std::to_string(COMMON::PLATFORM::identity()));
  std::ofstream file(probe);
  const Flag opened = file.is_open();
  file.close();
  std::error_code slip;
  std::filesystem::remove(probe, slip);
  return opened;
}

static String state(const String &directory) {
  if (writable(directory)) return directory;
  std::error_code slip;
  const String root = COMMON::PLATFORM::state();
  std::filesystem::path path = root.empty()
                                 ? std::filesystem::temp_directory_path(slip)
                                 : std::filesystem::path(root);
  if (path.empty()) return directory;
  path /= NAME;
  std::filesystem::create_directories(path, slip);
  return path.string();
}

const String COMMON::BINARY = COMMON::PLATFORM::executable();
const String COMMON::HOME = home(COMMON::BINARY);
const String COMMON::STATE = state(COMMON::HOME);
