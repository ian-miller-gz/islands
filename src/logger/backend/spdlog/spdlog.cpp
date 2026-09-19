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
#include <iostream>
#include <logger/backend/spdlog/spdlog.hpp>
#include <logger/filters.hpp>

#if SR_LOGGER_BACKEND == SR_SPDLOG

using LOGGER::Category;
using LOGGER::Level;
using Enum = spdlog::level::level_enum;

namespace {
auto sinkConsole = std::make_shared<spdlog::sinks::stdout_sink_mt>();
Vector<spdlog::sink_ptr> sinks;
std::shared_ptr<spdlog::logger> root;
Map<String, std::shared_ptr<spdlog::logger>> owned;

auto shared() -> Vector<spdlog::sink_ptr> & {
  if (sinks.empty())
    sinks = {
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        LOGGER::GET::path(), LOGGER::BACKEND::TRUNCATE),
      sinkConsole, LOGGER::BACKEND::tap()};
  return sinks;
}

auto rooted() -> spdlog::logger & {
  if (!root)
    root =
      std::make_shared<spdlog::logger>("", shared().begin(), shared().end());
  return *root;
}

void gate(spdlog::logger &logger, Level level) {
  logger.set_level(LOGGER::BACKEND::translate(level));
  logger.flush_on(spdlog::level::trace);
}
}  // namespace

auto LOGGER::BACKEND::translate(Level level) -> Enum {
  static constexpr Enum levels[] = {
    spdlog::level::critical, spdlog::level::critical, spdlog::level::critical,
    spdlog::level::err,      spdlog::level::warn,     spdlog::level::info,
    spdlog::level::info,     spdlog::level::debug,    spdlog::level::trace,
  };
  return levels[static_cast<int>(level)];
}

void LOGGER::initialize() {
  std::cout << std::unitbuf;
  for (auto &sink : shared()) sink->set_pattern(BACKEND::FORMAT);
  gate(rooted(), FILTERS::fallback);
}

void LOGGER::reset() {
  for (auto &sink : shared()) sink->set_pattern(BACKEND::FORMAT);
  gate(rooted(), FILTERS::fallback);
}

auto LOGGER::Category::getRoot() -> Category & {
  static Category anchor;
  if (!anchor.native) anchor.native = &rooted();
  return anchor;
}

auto LOGGER::get(const String &name) -> Category & {
  static Map<String, Category> registry;
  auto &category = registry[name];
  if (!category.native) {
    auto &logger = owned[name];
    logger =
      std::make_shared<spdlog::logger>(name, shared().begin(), shared().end());
    gate(*logger, FILTERS::fallback);
    category.native = logger.get();
  }
  Whole longest = 0;
  for (const auto &[prefix, level] : FILTERS::rules) {
    if (!name.starts_with(prefix) || prefix.size() < longest) continue;
    longest = prefix.size();
    gate(*static_cast<spdlog::logger *>(category.native), level);
  }
  return category;
}

#endif
