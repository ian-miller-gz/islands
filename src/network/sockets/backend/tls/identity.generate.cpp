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
#include <fcntl.h>
#include <mbedtls/ecp.h>
#include <mbedtls/x509_crt.h>
#include <unistd.h>

#include <cstring>
#include <network/sockets/tls.internal.hpp>

static constexpr STRING::Hot SINCE = "20250101000000";
static constexpr STRING::Hot UNTIL = "20551231235959";

static auto minted(mbedtls_pk_context *key) -> Flag {
  if (mbedtls_pk_setup(key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0)
    return false;
  return mbedtls_ecp_gen_key(
           MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(*key),
           mbedtls_ctr_drbg_random, NETWORK::TLS::GET::generator()) == 0;
}

static auto certified(mbedtls_pk_context *key, unsigned char *pem, size_t size)
  -> Flag {
  auto *rng = NETWORK::TLS::GET::generator();
  String name = String("CN=") + NETWORK::TLS::PEER;
  unsigned char serial[8];
  mbedtls_ctr_drbg_random(rng, serial, sizeof(serial));
  serial[0] &= 0x7F;
  mbedtls_x509write_cert paper;
  mbedtls_x509write_crt_init(&paper);
  mbedtls_x509write_crt_set_version(&paper, MBEDTLS_X509_CRT_VERSION_3);
  mbedtls_x509write_crt_set_md_alg(&paper, MBEDTLS_MD_SHA256);
  mbedtls_x509write_crt_set_subject_key(&paper, key);
  mbedtls_x509write_crt_set_issuer_key(&paper, key);
  Flag written =
    mbedtls_x509write_crt_set_subject_name(&paper, name.c_str()) == 0 &&
    mbedtls_x509write_crt_set_issuer_name(&paper, name.c_str()) == 0 &&
    mbedtls_x509write_crt_set_validity(&paper, SINCE, UNTIL) == 0 &&
    mbedtls_x509write_crt_set_serial_raw(&paper, serial, sizeof(serial)) == 0 &&
    mbedtls_x509write_crt_set_basic_constraints(&paper, 1, -1) == 0 &&
    mbedtls_x509write_crt_pem(
      &paper, pem, size, mbedtls_ctr_drbg_random, rng) == 0;
  mbedtls_x509write_crt_free(&paper);
  return written;
}

static auto persisted(
  const String &path, const unsigned char *text, mode_t mode) -> Flag {
  int file = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
  if (file == -1) return false;
  auto size = std::strlen(reinterpret_cast<STRING::Hot>(text));
  Flag whole = ::write(file, text, size) == static_cast<ssize_t>(size);
  ::close(file);
  return whole;
}

auto NETWORK::TLS::generate() -> Flag {
  if (!GET::generator()) return false;
  mbedtls_pk_context key;
  mbedtls_pk_init(&key);
  unsigned char secret[2048];
  unsigned char anchor[4096];
  Flag whole = minted(&key) &&
               mbedtls_pk_write_key_pem(&key, secret, sizeof(secret)) == 0 &&
               certified(&key, anchor, sizeof(anchor)) &&
               persisted(GET::secret(), secret, 0600) &&
               persisted(GET::anchor(), anchor, 0644);
  mbedtls_pk_free(&key);
  return whole;
}
