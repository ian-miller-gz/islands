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
#include <island/gui/parse.internal.hpp>
#include <island/gui/nga.internal.hpp>

namespace {

auto ported(const String &value, GUI::NGA::Port &port) -> Status {
  static constexpr STRING::Hot WORDS[] = {"none", "in", "out"};
  const auto comma = value.find(',');
  const String side = value.substr(0, comma);
  for (Whole at = 0; at < sizeof(WORDS) / sizeof(*WORDS); at += 1) {
    if (side != WORDS[at]) continue;
    port.side = static_cast<GUI::NGA::Port::Side>(at);
    port.kind = comma == String::npos ? "" : value.substr(comma + 1);
    return 0;
  }
  return 1;
}

auto panned(const String &value, GUI::NGA::Pan &pan) -> Status {
  const auto comma = value.find(',');
  if (comma == String::npos) return 1;
  GUI::NGA::Pan read;
  if (GUI::PARSE::read(value.substr(0, comma), read.x) != 0) return 1;
  if (GUI::PARSE::read(value.substr(comma + 1), read.y) != 0) return 1;
  pan = read;
  return 0;
}

auto held(const String &value, GUI::NGA::Pinned &pinned) -> Status {
  static constexpr STRING::Hot WORDS[] = {"none", "across", "down"};
  for (Whole at = 0; at < sizeof(WORDS) / sizeof(*WORDS); at += 1) {
    if (value != WORDS[at]) continue;
    pinned.axis = static_cast<GUI::NGA::Pinned::Axis>(at);
    return 0;
  }
  return 1;
}

auto zoomed(const String &value, GUI::NGA::Zoom &zoom) -> Status {
  const auto comma = value.find(',');
  GUI::NGA::Zoom read;
  if (GUI::PARSE::read(value.substr(0, comma), read.value) != 0) return 1;
  if (comma != String::npos)
    if (GUI::PARSE::read(value.substr(comma + 1), read.down) != 0) return 1;
  const auto scale = GUI::NGA::resolved(read);
  if (scale.w <= 0.0f || scale.h <= 0.0f) return 1;
  zoom = read;
  return 0;
}

}  // namespace

auto GUI::PARSE::boarded(const String &key) -> Flag {
  return key == "port" || key == "pan" || key == "zoom" || key == "pinned" ||
         key == "choosing";
}

static auto chosen(const String &value, GUI::NGA::Choosing &choosing)
  -> Status {
  if (value == "click") return void(choosing.band = false), 0;
  if (value == "band") return void(choosing.band = true), 0;
  return 1;
}

auto GUI::PARSE::board(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  if (key == "port") {
    NGA::Port port;
    return ::ported(value, port) != 0 ? 1 : NGA::set(document, id, port);
  }
  if (key == "pan") {
    NGA::Pan pan;
    return ::panned(value, pan) != 0 ? 1 : NGA::set(document, id, pan);
  }
  if (key == "pinned") {
    NGA::Pinned pinned;
    return ::held(value, pinned) != 0 ? 1 : NGA::set(document, id, pinned);
  }
  if (key == "choosing") {
    NGA::Choosing choosing;
    return ::chosen(value, choosing) != 0 ? 1
                                          : NGA::set(document, id, choosing);
  }
  NGA::Zoom zoom;
  return ::zoomed(value, zoom) != 0 ? 1 : NGA::set(document, id, zoom);
}

auto GUI::PARSE::board(
  Properties &properties, const String &key, const String &value) -> Status {
  if (key == "port") {
    NGA::Port port;
    if (::ported(value, port) != 0) return 1;
    properties.port = static_cast<Whole>(port.side);
    properties.kind = port.kind;
    return 0;
  }
  if (key == "pan") {
    NGA::Pan pan;
    if (::panned(value, pan) != 0) return 1;
    properties.pan = {pan.x, pan.y};
    return 0;
  }
  if (key == "pinned") {
    NGA::Pinned pinned;
    if (::held(value, pinned) != 0) return 1;
    properties.pinned = static_cast<Whole>(pinned.axis);
    return 0;
  }
  if (key == "choosing") {
    NGA::Choosing choosing;
    if (::chosen(value, choosing) != 0) return 1;
    properties.banded = choosing.band;
    return 0;
  }
  NGA::Zoom zoom;
  if (::zoomed(value, zoom) != 0) return 1;
  properties.zoom = NGA::resolved(zoom);
  return 0;
}
