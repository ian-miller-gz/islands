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
#include <mbedtls/entropy.h>
#include <mbedtls/x509_crt.h>
#include <psa/crypto.h>

#include <network/sockets/tls.internal.hpp>

auto NETWORK::TLS::GET::generator() -> mbedtls_ctr_drbg_context* {
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

auto NETWORK::TLS::GET::client() -> const mbedtls_ssl_config* {
  static mbedtls_ssl_config config;
  static mbedtls_x509_crt trust;
  static Flag ready = [] {
    if (!generator() || !identify()) return false;
    mbedtls_x509_crt_init(&trust);
    if (mbedtls_x509_crt_parse_file(&trust, anchor().c_str()) != 0)
      return false;
    mbedtls_ssl_config_init(&config);
    if (
      mbedtls_ssl_config_defaults(
        &config, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT) != 0)
      return false;
    mbedtls_ssl_conf_rng(&config, mbedtls_ctr_drbg_random, generator());
    mbedtls_ssl_conf_authmode(&config, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&config, &trust, nullptr);
    return true;
  }();
  return ready ? &config : nullptr;
}

auto NETWORK::TLS::GET::server() -> const mbedtls_ssl_config* {
  static mbedtls_ssl_config config;
  static mbedtls_x509_crt own;
  static mbedtls_pk_context key;
  static Flag ready = [] {
    if (!generator() || !identify()) return false;
    mbedtls_x509_crt_init(&own);
    mbedtls_pk_init(&key);
    if (
      mbedtls_x509_crt_parse_file(&own, anchor().c_str()) != 0 ||
      mbedtls_pk_parse_keyfile(
        &key, secret().c_str(), nullptr, mbedtls_ctr_drbg_random,
        generator()) != 0)
      return false;
    mbedtls_ssl_config_init(&config);
    if (
      mbedtls_ssl_config_defaults(
        &config, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT) != 0)
      return false;
    mbedtls_ssl_conf_rng(&config, mbedtls_ctr_drbg_random, generator());
    return mbedtls_ssl_conf_own_cert(&config, &own, &key) == 0;
  }();
  return ready ? &config : nullptr;
}
