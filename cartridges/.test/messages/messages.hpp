// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <network.hpp>
#include <string>

namespace MESSAGES {

constexpr STRING::Hot NAME = "messages";
constexpr STRING::Hot PUBLISH = "publish";
constexpr STRING::Hot SUBSCRIBE = "subscribe";
constexpr char TAB = '\t';

inline auto publish(const String &topic, const String &payload) -> String {
  return String(PUBLISH) + TAB + topic + TAB + payload + "\n";
}

inline auto subscribe(const String &topic) -> String {
  return String(SUBSCRIBE) + TAB + topic + "\n";
}

inline auto deliver(const String &topic, const String &payload) -> String {
  return topic + TAB + payload + "\n";
}

}  // namespace MESSAGES
