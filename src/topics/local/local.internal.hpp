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

#include <topics.hpp>

namespace TOPICS::SCHEDULE {

void purge();

}  // namespace TOPICS::SCHEDULE

namespace TOPICS::WIRE {

enum class Verb { NONE, SUBSCRIBE, PUBLISH };

struct Frame {
  Verb verb = Verb::NONE;
  String topic;
  Message message;
};

auto subscribe(STRING::Hot topic) -> String;
auto publish(const String &topic, const Message &message) -> String;
auto decode(const String &line) -> Frame;

}  // namespace TOPICS::WIRE

namespace TOPICS::LINK {

void pump();
void purge();

}  // namespace TOPICS::LINK

namespace TOPICS::BROKER {

void pump();
void purge();

}  // namespace TOPICS::BROKER
