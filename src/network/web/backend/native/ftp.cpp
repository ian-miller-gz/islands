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
#include <network/web/native.ftp.internal.hpp>

namespace NETWORK::WEB::NATIVE::FTP {
namespace {
constexpr Whole DONE = 200;
constexpr Whole WRONG = 300;

auto commanded(const Request &request, const Url &target) -> String {
  return (request.method == "GET" ? "RETR " : "STOR ") + target.path;
}

auto carry(Connection &data, const Request &request, Result &result) -> Flag {
  Flag moved = request.method == "GET" ? receive(data, result.body)
                                       : send(data, request.body);
  close(data);
  return moved;
}

auto converse(const Request &request, const Url &target, Connection &control)
  -> Result {
  if (!enter(control))
    return failed("the server refused the session", request.url);
  Connection data;
  if (!attach(control, target, data))
    return failed("the server opened no data connection", request.url);
  Reply started = speak(control, commanded(request, target));
  if (started.code >= DONE) {
    close(data);
    return {true, started.code, {}, {}, {}};
  }
  if (!started.code) {
    close(data);
    return failed("the server answered nothing", request.url);
  }
  Result result{};
  if (!carry(data, request, result))
    return failed("the data connection broke", request.url);
  Reply finished = hear(control);
  if (finished.code < DONE || finished.code >= WRONG)
    return failed(
      "the transfer did not complete (" + std::to_string(finished.code) + ")",
      request.url);
  result.ok = true;
  result.status = finished.code;
  return result;
}
}  // namespace

auto exchange(const Request &request, const Url &target) -> Result {
  Connection control;
  if (!open(target, control))
    return failed(
      "could not reach " + target.host + ":" + std::to_string(target.port),
      request.url);
  Result result = converse(request, target, control);
  speak(control, "QUIT");
  close(control);
  return result;
}
}  // namespace NETWORK::WEB::NATIVE::FTP
