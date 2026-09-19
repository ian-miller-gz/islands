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
#include <cstdio>
#include <logger/backend/spdlog/spdlog.hpp>

#if SR_LOGGER_BACKEND == SR_SPDLOG

using Enum = spdlog::level::level_enum;

void LOGGER::BACKEND::record(void *native, Enum level, const String &message) {
  static_cast<spdlog::logger *>(native)->log(level, message);
}

void LOGGER::BACKEND::record(
  void *native, Enum level, STRING::Hot format, va_list arguments) {
  va_list measure;
  va_copy(measure, arguments);
  int length = std::vsnprintf(nullptr, 0, format, measure);
  va_end(measure);
  if (length < 0) return;
  String message(static_cast<Whole>(length), '\0');
  std::vsnprintf(message.data(), message.size() + 1, format, arguments);
  static_cast<spdlog::logger *>(native)->log(level, message);
}

void LOGGER::Category::debug(const String &message) {
  BACKEND::record(native, spdlog::level::debug, message);
}
void LOGGER::Category::debug(STRING::Hot format, ...) {
  va_list arguments;
  va_start(arguments, format);
  BACKEND::record(native, spdlog::level::debug, format, arguments);
  va_end(arguments);
}

void LOGGER::Category::info(const String &message) {
  BACKEND::record(native, spdlog::level::info, message);
}
void LOGGER::Category::info(STRING::Hot format, ...) {
  va_list arguments;
  va_start(arguments, format);
  BACKEND::record(native, spdlog::level::info, format, arguments);
  va_end(arguments);
}

void LOGGER::Category::warn(const String &message) {
  BACKEND::record(native, spdlog::level::warn, message);
}
void LOGGER::Category::warn(STRING::Hot format, ...) {
  va_list arguments;
  va_start(arguments, format);
  BACKEND::record(native, spdlog::level::warn, format, arguments);
  va_end(arguments);
}

void LOGGER::Category::error(const String &message) {
  BACKEND::record(native, spdlog::level::err, message);
}
void LOGGER::Category::error(STRING::Hot format, ...) {
  va_list arguments;
  va_start(arguments, format);
  BACKEND::record(native, spdlog::level::err, format, arguments);
  va_end(arguments);
}

void LOGGER::Category::fatal(const String &message) {
  BACKEND::record(native, spdlog::level::critical, message);
}
void LOGGER::Category::fatal(STRING::Hot format, ...) {
  va_list arguments;
  va_start(arguments, format);
  BACKEND::record(native, spdlog::level::critical, format, arguments);
  va_end(arguments);
}

#endif
