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
#include <island/gui/nga.internal.hpp>
#include <island/gui/backend.hpp>
#include <algorithm>
#include <cmath>

namespace {

auto sign(Float travel) -> Float {
  return (travel > 0.0f ? 1.0f : 0.0f) - (travel < 0.0f ? 1.0f : 0.0f);
}

auto column(GUI::NGA::End out, GUI::NGA::End in, Float thickness) -> Float {
  const Float mid = (out.at.x + in.at.x) / 2.0f;
  const Float east = ::sign(in.at.x - out.at.x);
  const Float south = ::sign(in.at.y - out.at.y);
  const Float steps = static_cast<Float>(out.rank + in.rank);
  const Float stepped =
    mid - south * east * steps * GUI::NGA::PITCH * thickness;
  return std::clamp(
    stepped, std::min(out.at.x, in.at.x), std::max(out.at.x, in.at.x));
}

auto fan(GUI::NGA::End out, GUI::NGA::End in, Float thickness) -> Float {
  return static_cast<Float>(out.rank + in.rank) * GUI::NGA::PITCH * thickness;
}

auto channel(
  const GUI::NGA::Span &out, const GUI::NGA::Span &in, Float step,
  Float stub) -> Float {
  if (out.y + out.h <= in.y)
    return std::min((out.y + out.h + in.y) / 2.0f + step, in.y);
  if (in.y + in.h <= out.y)
    return std::min((in.y + in.h + out.y) / 2.0f + step, out.y);
  return std::max(out.y + out.h, in.y + in.h) + stub + step;
}

}  // namespace

void GUI::NGA::route(End out, End in, Float thickness, Span *run) {
  const Float half = thickness / 2.0f;
  const Float stub = STUB * thickness;
  const Float step = ::fan(out, in, thickness);
  const Flag backward = in.at.x - out.at.x < 2.0f * stub;
  const Float mid = ::column(out, in, thickness);
  const Float east = backward ? out.at.x + stub + step : mid;
  const Float west = backward ? in.at.x - stub - step : mid;
  const Float row = backward ? ::channel(out.box, in.box, step, stub) : in.at.y;
  const Position corners[SEGMENTS][2] = {
    {out.at, {east, out.at.y}},
    {{east, out.at.y}, {east, row}},
    {{east, row}, {west, row}},
    {{west, row}, {west, in.at.y}},
    {{west, in.at.y}, in.at}};
  for (Whole at = 0; at < SEGMENTS; at += 1) {
    const auto &a = corners[at][0];
    const auto &b = corners[at][1];
    run[at] = {
      std::min(a.x, b.x) - half, std::min(a.y, b.y) - half,
      std::abs(b.x - a.x) + thickness, std::abs(b.y - a.y) + thickness};
  }
}

auto GUI::NGA::centre(Handle document, Whole index) -> Position {
  const auto corner = BACKEND::origin(document, index);
  const auto size = BACKEND::measured(document, index);
  return {corner.x + size.w / 2.0f, corner.y + size.h / 2.0f};
}

static auto ratio(Float authored, Float compiled) -> Float {
  if (authored <= 0.0f || compiled <= 0.0f) return 1.0f;
  return compiled / authored;
}

auto GUI::NGA::scaled(Handle document, Whole index) -> Extent {
  const auto &properties = STATE::documents[document].nodes[index].properties;
  const auto compiled = BACKEND::measured(document, index);
  return {
    ::ratio(properties.extent.w, compiled.w),
    ::ratio(properties.extent.h, compiled.h)};
}
