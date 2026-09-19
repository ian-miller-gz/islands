// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace LAUNCHER {

constexpr Float INDENT = 14;
constexpr Float STEM = 7;
constexpr Float NAME = 14;

struct Area {
  STRING::Hot id;
  STRING::Hot directory;
};

constexpr Area AREAS[] = {
  {IDS::GENERAL, HOME}, {IDS::CORE, "cartridges/.core"}};

struct Scene {
  STRING::Hot call;
  STRING::Hot panel;
};

constexpr Scene SCENES[] = {{IDS::LIBRARY, IDS::MAIN}};

void refresh();

namespace LISTING {
void refresh();
}  // namespace LISTING
namespace LINEWORK {
constexpr Whole RULES = 12;
constexpr Whole TICKS = 24;
void refresh();
}  // namespace LINEWORK
namespace CREW {
void refresh();
}  // namespace CREW
namespace CARDS {
void refresh();
}  // namespace CARDS
namespace STAGE {
void refresh();
}  // namespace STAGE
namespace TOOLBAR {
void refresh();
}  // namespace TOOLBAR
namespace PREVIEW {
void refresh();
}  // namespace PREVIEW
namespace FOOTER {
void refresh();
}  // namespace FOOTER

void slide(STRING::Hot id, Float x);
void span(STRING::Hot id, Float y, Float h);

namespace GET {
auto columns(Whole depth) -> Whole;
auto noticing() -> Flag;
auto setting() -> Flag;
auto picked() -> Whole;
auto shortened(const String &text, Whole columns) -> String;
auto plain(const String &text) -> String;
auto worn(const Entry &entry) -> String;
auto folded(const Entry &entry) -> GUI::Fold;
}  // namespace GET
}  // namespace LAUNCHER

namespace LAUNCHER::STYLES {
constexpr STRING::Hot SHEET = "sheet";
constexpr STRING::Hot ENTRY = "entry";
constexpr STRING::Hot PLACE = "place";
constexpr STRING::Hot CALL = "call";
constexpr STRING::Hot INK = "ink";
constexpr STRING::Hot SELECTED = "selected";
constexpr STRING::Hot STILL = "still";
constexpr STRING::Hot PLATE = "plate";
constexpr STRING::Hot FOLDED = "folded";
constexpr STRING::Hot UNFOLDED = "unfolded";
constexpr STRING::Hot LEAF = "leaf";
constexpr STRING::Hot SLAT = "slat";
constexpr STRING::Hot TRACE = "trace";
}  // namespace LAUNCHER::STYLES
