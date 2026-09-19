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
#include <logger/backend/spdlog/spdlog.hpp>
#include <logger/tap.internal.hpp>

#include <spdlog/sinks/base_sink.h>

#if SR_LOGGER_BACKEND == SR_SPDLOG

namespace {
class Sink final : public spdlog::sinks::base_sink<std::mutex> {
 protected:
  void sink_it_(const spdlog::details::log_msg &message) override {
    spdlog::memory_buf_t formatted;
    formatter_->format(message, formatted);
    String line(formatted.data(), formatted.size());
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
      line.pop_back();
    LOGGER::TAP::push(line);
  }
  void flush_() override {}
};
}  // namespace

auto LOGGER::BACKEND::tap() -> spdlog::sink_ptr {
  static auto sink = std::make_shared<Sink>();
  return sink;
}

#endif
