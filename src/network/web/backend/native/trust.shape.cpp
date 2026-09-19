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
#include <mbedtls/ssl.h>
#include <psa/crypto.h>

#include <network/web/native.internal.hpp>

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

auto NETWORK::WEB::NATIVE::shape(
  mbedtls_ssl_config &config, mbedtls_x509_crt &roots) -> Flag {
  if (!generator()) return false;
  mbedtls_ssl_config_init(&config);
  if (
    mbedtls_ssl_config_defaults(
      &config, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
      MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    return false;
  mbedtls_ssl_conf_rng(&config, mbedtls_ctr_drbg_random, generator());
  mbedtls_ssl_conf_authmode(&config, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(&config, &roots, nullptr);
  return true;
}
