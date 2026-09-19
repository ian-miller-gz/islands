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
#pragma once
#include <imgui/imgui.h>
#include <island/graphics/color.hpp>

namespace ISLANDS::THEME {

constexpr ImU32 ACCENT = IM_COL32(0x2b, 0x6c, 0xb0, 0xff);
constexpr ImU32 MUTED = IM_COL32(0x71, 0x80, 0x96, 0xff);
constexpr ImU32 LIVE = IM_COL32(0x38, 0xa1, 0x69, 0xff);
constexpr ImU32 REST = IM_COL32(0xa0, 0xae, 0xc0, 0xff);

constexpr GFX::Color BACKDROP = {0.95f, 0.96f, 0.97f, 1.0f};

struct Swatch {
  ImGuiCol slot;
  ImU32 color;
};

constexpr Swatch PALETTE[] = {
  {ImGuiCol_Text, IM_COL32(0x1a, 0x20, 0x2c, 0xff)},
  {ImGuiCol_TextDisabled, IM_COL32(0xa0, 0xae, 0xc0, 0xff)},
  {ImGuiCol_WindowBg, IM_COL32(0xf2, 0xf4, 0xf7, 0xff)},
  {ImGuiCol_ChildBg, IM_COL32(0xf2, 0xf4, 0xf7, 0xff)},
  {ImGuiCol_PopupBg, IM_COL32(0xff, 0xff, 0xff, 0xff)},
  {ImGuiCol_Border, IM_COL32(0xcb, 0xd5, 0xe0, 0xff)},
  {ImGuiCol_FrameBg, IM_COL32(0xff, 0xff, 0xff, 0xff)},
  {ImGuiCol_FrameBgHovered, IM_COL32(0xed, 0xf2, 0xf7, 0xff)},
  {ImGuiCol_FrameBgActive, IM_COL32(0xe2, 0xe8, 0xf0, 0xff)},
  {ImGuiCol_TitleBg, IM_COL32(0xe2, 0xe8, 0xf0, 0xff)},
  {ImGuiCol_TitleBgActive, IM_COL32(0xcf, 0xe0, 0xf2, 0xff)},
  {ImGuiCol_TitleBgCollapsed, IM_COL32(0xe2, 0xe8, 0xf0, 0xff)},
  {ImGuiCol_MenuBarBg, IM_COL32(0xe2, 0xe8, 0xf0, 0xff)},
  {ImGuiCol_ScrollbarBg, IM_COL32(0xed, 0xf2, 0xf7, 0xff)},
  {ImGuiCol_ScrollbarGrab, IM_COL32(0xcb, 0xd5, 0xe0, 0xff)},
  {ImGuiCol_ScrollbarGrabHovered, IM_COL32(0xa0, 0xae, 0xc0, 0xff)},
  {ImGuiCol_ScrollbarGrabActive, IM_COL32(0x71, 0x80, 0x96, 0xff)},
  {ImGuiCol_CheckMark, ACCENT},
  {ImGuiCol_SliderGrab, ACCENT},
  {ImGuiCol_SliderGrabActive, IM_COL32(0x1e, 0x4e, 0x8c, 0xff)},
  {ImGuiCol_Button, IM_COL32(0xe2, 0xe8, 0xf0, 0xff)},
  {ImGuiCol_ButtonHovered, IM_COL32(0xcf, 0xe0, 0xf2, 0xff)},
  {ImGuiCol_ButtonActive, IM_COL32(0x9e, 0xc5, 0xe8, 0xff)},
  {ImGuiCol_Header, IM_COL32(0xcf, 0xe0, 0xf2, 0xff)},
  {ImGuiCol_HeaderHovered, IM_COL32(0xb7, 0xd3, 0xee, 0xff)},
  {ImGuiCol_HeaderActive, IM_COL32(0x9e, 0xc5, 0xe8, 0xff)},
  {ImGuiCol_Separator, IM_COL32(0xcb, 0xd5, 0xe0, 0xff)},
  {ImGuiCol_Tab, IM_COL32(0xe2, 0xe8, 0xf0, 0xff)},
  {ImGuiCol_TabHovered, IM_COL32(0xb7, 0xd3, 0xee, 0xff)},
  {ImGuiCol_TabSelected, IM_COL32(0xcf, 0xe0, 0xf2, 0xff)},
};

inline void apply() {
  ImGuiStyle &style = ImGui::GetStyle();
  for (const auto &swatch : PALETTE)
    style.Colors[swatch.slot] = ImGui::ColorConvertU32ToFloat4(swatch.color);
  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 3.0f;
  style.GrabRounding = 3.0f;
  style.TabRounding = 3.0f;
  style.WindowBorderSize = 1.0f;
  style.FrameBorderSize = 1.0f;
}

}  // namespace ISLANDS::THEME
