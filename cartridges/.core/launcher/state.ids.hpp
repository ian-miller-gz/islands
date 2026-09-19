// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace LAUNCHER::IDS {
constexpr STRING::Hot ABOUT = "about";
constexpr STRING::Hot NOTICE = "notice";
constexpr STRING::Hot DISMISS = "noticeclose";
constexpr STRING::Hot GENERAL = "general";
constexpr STRING::Hot CORE = "core";
constexpr STRING::Hot RAIL = "rail";
constexpr STRING::Hot LIBRARY = "library";
constexpr STRING::Hot MAIN = "main";
constexpr STRING::Hot BROWSER = "browser";
constexpr STRING::Hot SHEET = "preview";
constexpr STRING::Hot WATCH = "watch";
constexpr STRING::Hot NOTE = "note";
constexpr STRING::Hot BARE = "bare";
constexpr STRING::Hot QUIET = "quiet";
constexpr STRING::Hot LAUNCH = "launch";
constexpr STRING::Hot GEAR = "gear";
constexpr STRING::Hot SETUP = "setup";
constexpr STRING::Hot SETTLE = "setupclose";
constexpr STRING::Hot TITLED = "setupname";
constexpr STRING::Hot BOX = "box";
constexpr STRING::Hot NAMED = "pname";
constexpr STRING::Hot SUMMARY = "psummary";
constexpr Whole FACTS = 3;
inline auto fact(Whole at) -> String { return "pfact" + std::to_string(at); }
constexpr STRING::Hot TREE = "tree";
constexpr STRING::Hot CREW = "crew";
inline auto tick(Whole at) -> String { return "tick" + std::to_string(at); }
inline auto rule(Whole at) -> String { return "rule" + std::to_string(at); }
inline auto box(Whole at) -> String { return BOX + std::to_string(at); }
inline auto word(Whole at) -> String { return "boxword" + std::to_string(at); }
inline auto item(STRING::Hot list, Whole at) -> String {
  return String(list) + "." + std::to_string(at);
}
inline auto note(Whole at) -> String { return "note" + std::to_string(at); }
}  // namespace LAUNCHER::IDS
