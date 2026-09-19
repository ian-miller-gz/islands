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

namespace NETWORK::WEB {

struct Header {
  String name;
  String value;
};

struct Request {
  String url;
  String method;
  Vector<Header> headers;
  String body;
};

struct Result {
  Flag ok;
  Whole status;
  String body;
  String error;
  String redirect;
};

auto get(const String &url) -> Result;

auto put(const String &url, const String &body) -> Result;

auto post(const String &url, const String &body) -> Result;

auto transfer(const Request &request) -> Result;

void adopt(const Vector<String> &declarations, const String &home = {});

}  // namespace NETWORK::WEB
