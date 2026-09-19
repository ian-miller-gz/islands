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
#include <cstdarg>
#include <logger/backend/selection.hpp>
#include <logger/logger.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#if SR_LOGGER_BACKEND == SR_SPDLOG

namespace LOGGER::BACKEND {

inline constexpr STRING::Hot FORMAT = "%M:%S %-8l %n - %v";
inline constexpr bool TRUNCATE = true;

auto translate(Level level) -> spdlog::level::level_enum;

auto tap() -> spdlog::sink_ptr;

void record(
  void *native, spdlog::level::level_enum level, const String &message);
void record(
  void *native, spdlog::level::level_enum level, STRING::Hot format,
  va_list arguments);

}  // namespace LOGGER::BACKEND

#endif
