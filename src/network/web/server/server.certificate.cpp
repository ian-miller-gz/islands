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
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/pk.h>
#include <mbedtls/x509_crt.h>
#include <psa/crypto.h>

#include <logger.hpp>
#include <network/web/server.link.internal.hpp>
#define LOGGER_CATEGORY "~/network::serve"

namespace NETWORK::WEB::SERVER {
namespace {
auto generator() -> mbedtls_ctr_drbg_context * {
  static mbedtls_entropy_context entropy;
  static mbedtls_ctr_drbg_context source;
  static Flag seeded = [] {
    if (psa_crypto_init() != PSA_SUCCESS) return false;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&source);
    return mbedtls_ctr_drbg_seed(
             &source, mbedtls_entropy_func, &entropy, nullptr, 0) == 0;
  }();
  return seeded ? &source : nullptr;
}
}  // namespace

auto certificate(const Declared &declared) -> const mbedtls_ssl_config * {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  static mbedtls_ssl_config role;
  static mbedtls_x509_crt own;
  static mbedtls_pk_context key;
  static String read;
  const String named = declared.certificate + " " + declared.key;
  if (read == named) return &role;
  if (!generator()) return nullptr;
  if (!read.empty()) {
    mbedtls_ssl_config_free(&role);
    mbedtls_x509_crt_free(&own);
    mbedtls_pk_free(&key);
    read.clear();
  }
  mbedtls_x509_crt_init(&own);
  mbedtls_pk_init(&key);
  if (mbedtls_x509_crt_parse_file(&own, declared.certificate.c_str()) != 0) {
    logger.error("Serving refused: cannot read " + declared.certificate);
    return nullptr;
  }
  if (
    mbedtls_pk_parse_keyfile(
      &key, declared.key.c_str(), nullptr, mbedtls_ctr_drbg_random,
      generator()) != 0) {
    logger.error("Serving refused: cannot read " + declared.key);
    return nullptr;
  }
  mbedtls_ssl_config_init(&role);
  if (
    mbedtls_ssl_config_defaults(
      &role, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM,
      MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    return nullptr;
  mbedtls_ssl_conf_rng(&role, mbedtls_ctr_drbg_random, generator());
  if (mbedtls_ssl_conf_own_cert(&role, &own, &key) != 0) return nullptr;
  read = named;
  return &role;
}

}  // namespace NETWORK::WEB::SERVER
