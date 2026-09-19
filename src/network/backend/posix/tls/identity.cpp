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
#include <filesystem>
#include <logger.hpp>
#include <network/backend/posix/home.internal.hpp>
#include <network/backend/posix/tls.internal.hpp>
#define LOGGER_CATEGORY "~/network::identity"

auto NETWORK::TLS::GET::anchor() -> String {
  return HOME::directory() + "/" + PEER + ".crt";
}

auto NETWORK::TLS::GET::secret() -> String {
  return HOME::directory() + "/" + PEER + ".key";
}

auto NETWORK::TLS::identify() -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  std::error_code trouble;
  if (
    std::filesystem::exists(GET::anchor(), trouble) &&
    std::filesystem::exists(GET::secret(), trouble))
    return true;
  const String home = HOME::directory();
  std::filesystem::create_directory(home, trouble);
  std::filesystem::permissions(
    home, std::filesystem::perms::owner_all, trouble);
  if (!generate()) {
    logger.debug(
      "Cannot mint the dev identity under %s", HOME::directory().c_str());
    return false;
  }
  logger.debug("Dev identity minted: %s", GET::anchor().c_str());
  return true;
}
