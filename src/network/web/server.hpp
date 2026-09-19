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

namespace NETWORK::WEB::SERVER {

struct Request {
  String reader;
  String path;
  String query;
};

struct Response {
  Whole status = 200;
  String type = "text/html; charset=utf-8";
  String body;
};

using Handler = auto (*)(const Request &) -> Response;

void adopt(const Vector<String> &declarations, const String &home = {});

auto open(Handler answering) -> Flag;

void process();

void close();

void grant(const String &reader, const String &secret);

void revoke(const String &reader);

namespace GET {
auto serving() -> Flag;
auto wire() -> String;
auto readers() -> Vector<String>;
}  // namespace GET

}  // namespace NETWORK::WEB::SERVER
