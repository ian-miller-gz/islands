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
#include <mbedtls/x509_crt.h>

#include <logger.hpp>
#include <network/web/native.internal.hpp>
#include <network/web/web.internal.hpp>
#define LOGGER_CATEGORY "~/network::web"

namespace NETWORK::WEB::NATIVE {
namespace {
struct Role {
  mbedtls_x509_crt anchor;
  mbedtls_ssl_config config;
  Flag ready = false;
};

Map<String, Role> roles;

void mint(const String &certificate, Role &role) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  mbedtls_x509_crt_init(&role.anchor);
  role.ready =
    mbedtls_x509_crt_parse_file(&role.anchor, certificate.c_str()) == 0 &&
    shape(role.config, role.anchor);
  if (!role.ready)
    logger.error(
      "Cannot read the declared certificate \"" + certificate +
      "\"; every secured transfer to the host that named it will fail.");
}
}  // namespace

auto GET::trust(const String &certificate) -> const mbedtls_ssl_config * {
  if (certificate.empty()) return trust();
  const auto seat = roles.find(certificate);
  if (seat == roles.end()) return nullptr;
  return seat->second.ready ? &seat->second.config : nullptr;
}
}  // namespace NETWORK::WEB::NATIVE

void NETWORK::WEB::warmed(const Vector<String> &certificates) {
  using namespace NETWORK::WEB::NATIVE;
  for (auto &[path, role] : roles) {
    mbedtls_ssl_config_free(&role.config);
    mbedtls_x509_crt_free(&role.anchor);
  }
  roles.clear();
  for (const String &certificate : certificates)
    if (!certificate.empty()) {
      auto [seat, fresh] = roles.try_emplace(certificate);
      if (fresh) mint(certificate, seat->second);
    }
}
