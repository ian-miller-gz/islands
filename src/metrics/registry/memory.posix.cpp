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
#include <metrics/registry/memory.internal.hpp>

#include <unistd.h>

#include <cstdio>

namespace {
constexpr int FIELDS = 2;
constexpr STRING::Hot STATM = "/proc/self/statm";
}  // namespace

auto METRICS::resident() -> Sample {
  std::FILE *statm = std::fopen(STATM, "r");
  if (!statm) return 0;
  long total = 0, pages = 0;
  const int read = std::fscanf(statm, "%ld %ld", &total, &pages);
  std::fclose(statm);
  if (read != FIELDS) return 0;
  static const long PAGE = sysconf(_SC_PAGESIZE);
  return static_cast<Sample>(pages) * static_cast<Sample>(PAGE);
}
