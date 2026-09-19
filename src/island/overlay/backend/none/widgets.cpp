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
#include <island.hpp>
#include <common.hpp>

#if SR_OVERLAY_BACKEND == SR_NONE

void OVERLAY::text(STRING::Hot) {}
void OVERLAY::dim(STRING::Hot) {}
void OVERLAY::tint(STRING::Hot, Tint) {}
void OVERLAY::wrapped(STRING::Hot) {}

auto OVERLAY::button(STRING::Hot) -> Flag { return false; }
auto OVERLAY::button(STRING::Hot, Small) -> Flag { return false; }
auto OVERLAY::selectable(STRING::Hot, Flag) -> Flag { return false; }
auto OVERLAY::checkbox(STRING::Hot, Flag &) -> Flag { return false; }

auto OVERLAY::field(STRING::Hot, String &) -> Flag { return false; }
auto OVERLAY::field(STRING::Hot, Whole &) -> Flag { return false; }
auto OVERLAY::field(STRING::Hot, Float &, Float &) -> Flag { return false; }

void OVERLAY::sameline() {}
void OVERLAY::separator() {}
void OVERLAY::spacing() {}
void OVERLAY::follow() {}

auto OVERLAY::item(STRING::Hot) -> Flag { return false; }
void OVERLAY::summon(STRING::Hot) {}
void OVERLAY::dismiss() {}

OVERLAY::Panel::Panel(STRING::Hot) : open(false) {}
OVERLAY::Panel::~Panel() {}
OVERLAY::Region::Region(STRING::Hot) : open(false) {}
OVERLAY::Region::~Region() {}
OVERLAY::Scope::Scope(STRING::Hot) {}
OVERLAY::Scope::Scope(Integer) {}
OVERLAY::Scope::~Scope() {}
OVERLAY::Menubar::Menubar() : open(false) {}
OVERLAY::Menubar::~Menubar() {}
OVERLAY::Menu::Menu(STRING::Hot) : open(false) {}
OVERLAY::Menu::~Menu() {}
OVERLAY::Modal::Modal(STRING::Hot) : open(false) {}
OVERLAY::Modal::~Modal() {}

#endif
