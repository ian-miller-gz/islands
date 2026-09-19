// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <network.hpp>
#include <string>

namespace ROSTER {

constexpr STRING::Hot NAME = "roster";
constexpr STRING::Hot HELLO = "hello";
constexpr STRING::Hot LIST = "list";
constexpr char TAB = '\t';
constexpr Whole FIELDS = 4;

inline auto greet(
  Integer pid, const String &name, const String &bundle,
  const NETWORK::Wire &self) -> String {
  return String(HELLO) + TAB + std::to_string(pid) + TAB + name + TAB + bundle +
         TAB + self.address + "\n";
}

}  // namespace ROSTER
