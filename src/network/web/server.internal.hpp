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

#include <common.hpp>
#include <network/web/server.hpp>

namespace NETWORK::WEB::SERVER {

constexpr Whole LINKS = 8;
constexpr Whole HEAD = 8u << 10;
constexpr Integer IDLE = 10000;
constexpr STRING::Hot REALM = "islands";
constexpr STRING::Hot BREAK = "\r\n\r\n";

struct Declared {
  String address;
  Whole port = 0;
  String certificate;
  String key;
};

auto declared() -> const Declared &;

auto resolve(const String &credential) -> String;

void disarm();

auto bind(const Declared &lane, Handler answering) -> Flag;

void advance();

void release();

auto bound() -> String;

}  // namespace NETWORK::WEB::SERVER
