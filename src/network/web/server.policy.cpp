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
#include <logger.hpp>
#include <network/web/server.internal.hpp>
#include <optional>
#include <sstream>
#define LOGGER_CATEGORY "~/network::serve"

namespace {
NETWORK::WEB::SERVER::Declared lane;

constexpr STRING::Hot GRAMMAR = "\"<address>:<port> as <certificate> <key>\".";

auto hang(const String &file, const String &home) -> String {
  if (file.empty() || file.starts_with("/")) return file;
  return home + file;
}

auto declare(const String &line)
  -> std::optional<NETWORK::WEB::SERVER::Declared> {
  std::istringstream words(line);
  NETWORK::WEB::SERVER::Declared read;
  String wire;
  String preposition;
  if (!(words >> wire >> preposition >> read.certificate >> read.key))
    return std::nullopt;
  if (preposition != "as" || words >> preposition) return std::nullopt;
  const auto colon = wire.rfind(':');
  if (colon == String::npos || colon == 0) return std::nullopt;
  read.address = wire.substr(0, colon);
  const String port = wire.substr(colon + 1);
  if (port.find_first_not_of("0123456789") != String::npos || port.empty())
    return std::nullopt;
  read.port = STRING::number<Whole>(port);
  if (read.port == 0 || read.port > 65535) return std::nullopt;
  return read;
}
}  // namespace

void NETWORK::WEB::SERVER::adopt(
  const Vector<String> &declarations, const String &home) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  lane = {};
  for (const auto &line : declarations)
    if (!lane.address.empty())
      logger.error(
        "Ignoring extra serve declaration \"" + line +
        "\"; one lane is served, and " + lane.address + " already holds it.");
    else if (auto read = declare(line)) {
      read->certificate = hang(read->certificate, home);
      read->key = hang(read->key, home);
      lane = *read;
    } else
      logger.error(
        "Ignoring malformed serve declaration \"" + line + "\"; expected " +
        GRAMMAR);
}

auto NETWORK::WEB::SERVER::declared() -> const Declared & { return lane; }
