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
#include <island/terminal/backend/unix/listen.internal.hpp>
#include <island/terminal/clear.hpp>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/terminal::listen"

#if SR_LISTEN == SR_UNIX

static String buffer;

void TERMINAL::BACKEND::LISTEN::send(const String &text) {
  if (session == NETWORK::NONE) return;
  if (!NETWORK::SESSIONS::send(session, text)) drop();
}

namespace {
struct Socket final : SHELL::Session {
  void print(const String &line) override {
    TERMINAL::BACKEND::LISTEN::send(line + "\n");
  }
  void clear() override { TERMINAL::BACKEND::LISTEN::send(TERMINAL::CLEAR); }
  void close() override { TERMINAL::BACKEND::LISTEN::drop(); }
};
}  // namespace

static void respond(const String &line) {
  Socket socket;
  if (TERMINAL::BACKEND::LISTEN::intercept(socket, line)) return;
  SHELL::ENGINE::dispatch(socket, line);
}

void TERMINAL::BACKEND::LISTEN::serve() {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  Flag alive = NETWORK::SESSIONS::receive(session, buffer);
  if (!alive && !buffer.empty() && buffer.back() != '\n') buffer += '\n';
  for (auto cut = buffer.find('\n'); cut != String::npos;
       cut = buffer.find('\n')) {
    String line = buffer.substr(0, cut);
    buffer.erase(0, cut + 1);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    logger.debug("Command: %s", line.c_str());
    respond(line);
    if (session == NETWORK::NONE) return;
  }
  if (!alive) drop();
}

void TERMINAL::BACKEND::LISTEN::drop() {
  if (streaming) LOGGER::TAP::close();
  streaming = false;
  if (session == NETWORK::NONE) return;
  NETWORK::SESSIONS::destroy(session);
  session = NETWORK::NONE;
  buffer.clear();
}

#endif
