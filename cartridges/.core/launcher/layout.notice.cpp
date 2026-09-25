// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

namespace LAUNCHER {
namespace {
constexpr STRING::Hot NOTICED = "Islands Beta - ver. ";
}  // namespace
}  // namespace LAUNCHER

void LAUNCHER::notice() {
  GUI::set(
    document, IDS::NOTICELINE, GUI::Text{String(NOTICED) + ENGINE::VERSION});
}
