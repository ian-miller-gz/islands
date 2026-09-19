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
#include <cartridge/relations.internal.hpp>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/cartridge::relations"

void RELATIONS::drop(Whole process) {
  for (Whole at = 0; at < children.size(); ++at)
    if (children[at].process == process)
      return static_cast<void>(children.erase(children.begin() + at));
}

auto RELATIONS::GET::alive(Whole process) -> Flag {
  for (const auto &child : children)
    if (child.process == process) return true;
  return false;
}

void RELATIONS::kill(Whole process) {
  if (!GET::alive(process)) return;
  SPAWN::end(process);
  join(process);
}

void RELATIONS::join(Whole process) {
  if (!GET::alive(process)) return;
  SPAWN::reap(process);
  drop(process);
}

void RELATIONS::process() {
  for (Whole at = 0; at < children.size();)
    if (SPAWN::ended(children[at].process)) {
      static auto &logger = LOGGER::get(LOGGER_CATEGORY);
      logger.info("Owned %s exited.", children[at].bundle.c_str());
      children.erase(children.begin() + at);
    } else {
      ++at;
    }
}

void RELATIONS::close() {
  for (const auto &child : children) SPAWN::end(child.process);
  for (const auto &child : children) SPAWN::reap(child.process);
  children.clear();
}
