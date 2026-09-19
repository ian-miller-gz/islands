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
#include <island/graphics/capture.hpp>
#include <logger.hpp>

String GFX::CAPTURE::path;
Flag GFX::CAPTURE::armed = false;

auto GFX::CAPTURE::write(
  Whole width, Whole height, const Byte *pixels, Flag swapped) -> Status {
  constexpr Whole DEPTH = 255;
  constexpr Whole COLORS = 3;
  static auto &logger = LOGGER::get("~/island/graphics::capture");
  IO::STREAMS::Output file(path, std::ios::binary);
  file << "P6\n" << width << " " << height << "\n" << DEPTH << "\n";
  for (Whole i = 0; i < width * height; i += 1) {
    const Byte *pixel = pixels + i * CHANNELS;
    if (swapped) {
      file.put(pixel[2]).put(pixel[1]).put(pixel[0]);
    } else {
      file.write(pixel, COLORS);
    }
  }
  armed = false;
  file.close();
  if (file.fail()) {
    logger.error("Cannot write the capture: %s.", path.c_str());
    return 1;
  }
  logger.info("Frame captured to %s", path.c_str());
  return 0;
}
