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
#include <network/web.hpp>
#include <network/web/backend/selection.hpp>
#include <network/web/url.internal.hpp>
#include <network/web/web.internal.hpp>
#include <sstream>
#define LOGGER_CATEGORY "~/network::web"

namespace {
struct Allowance {
  Vector<String> schemes;
  String host;
  String certificate;
};
Vector<Allowance> allowed;

constexpr STRING::Hot GRAMMAR =
  "\"<scheme> [scheme...] to <host> [trusting <certificate>]\".";

auto declare(const String &line) -> std::optional<Allowance> {
  std::istringstream words(line);
  Allowance rule;
  String word;
  while (words >> word && word != "to") rule.schemes.push_back(word);
  words >> rule.host;
  if (words >> word && (word != "trusting" || !(words >> rule.certificate)))
    return std::nullopt;
  if (rule.schemes.empty() || rule.host.empty()) return std::nullopt;
  return rule;
}

auto resolve(const String &certificate, const String &home) -> String {
  if (certificate.empty() || certificate.starts_with("/")) return certificate;
  return home + certificate;
}
}  // namespace

void NETWORK::WEB::adopt(
  const Vector<String> &declarations, const String &home) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  allowed.clear();
  for (const auto &line : declarations)
    if (auto rule = declare(line)) {
      rule->certificate = resolve(rule->certificate, home);
      allowed.push_back(*rule);
    } else
      logger.error(
        "Ignoring malformed web declaration \"" + line + "\"; expected " +
        GRAMMAR);
#if SR_WEB_BACKEND == SR_NATIVE
  Vector<String> anchors;
  for (const auto &rule : allowed)
    if (!rule.certificate.empty()) anchors.push_back(rule.certificate);
  warmed(anchors);
#endif
}

auto NETWORK::WEB::screen(const String &url) -> std::optional<String> {
  auto target = dissect(url);
  if (target.scheme.empty() || target.host.empty())
    return "web transfer refused: malformed URL " + url;
  if (allowed.empty())
    return "web transfer refused: no web reach declared (the manifest web: "
           "key or --web); requested " +
           url;
  const String &scheme = target.scheme;
  for (const auto &rule : allowed)
    if (rule.host == target.host)
      for (const auto &allow : rule.schemes)
        if (allow == scheme) return std::nullopt;
  return "web transfer refused: " + scheme + "://" + target.host +
         " is not in the declared allowlist; requested " + url;
}

auto NETWORK::WEB::GET::certificate(const String &host) -> String {
  for (const auto &rule : allowed)
    if (rule.host == host && !rule.certificate.empty()) return rule.certificate;
  return {};
}
