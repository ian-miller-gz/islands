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
#include <network/web/backend/selection.hpp>

#if SR_WEB_BACKEND == SR_NATIVE
#include <mbedtls/x509_crt.h>

#include <network/web/native.internal.hpp>
#include <wincrypt.h>

namespace {
constexpr const wchar_t *ROOTS = L"ROOT";

auto rooted(mbedtls_x509_crt *roots) -> Flag {
  mbedtls_x509_crt_init(roots);
  HCERTSTORE store = ::CertOpenSystemStoreW(0, ROOTS);
  if (!store) return false;
  Flag any = false;
  PCCERT_CONTEXT paper = nullptr;
  while ((paper = ::CertEnumCertificatesInStore(store, paper)) != nullptr)
    any = mbedtls_x509_crt_parse_der(
            roots, paper->pbCertEncoded, paper->cbCertEncoded) == 0 ||
          any;
  ::CertCloseStore(store, 0);
  return any;
}
}  // namespace

auto NETWORK::WEB::NATIVE::GET::trust() -> const mbedtls_ssl_config * {
  static mbedtls_ssl_config config;
  static mbedtls_x509_crt roots;
  static Flag ready = [] { return rooted(&roots) && shape(config, roots); }();
  return ready ? &config : nullptr;
}
#endif
