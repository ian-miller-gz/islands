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
#include <island/gui/events.internal.hpp>
#include <algorithm>

namespace STATE = GUI::STATE;

auto GUI::GET::hover(Handle document) -> STRING::Cold {
  if (!live(document)) return {};
  const auto &entry = STATE::documents[document];
  const auto hovered = entry.pointer.hovered;
  if (hovered == PARENTLESS || !entry.nodes[hovered].live) return {};
  return entry.nodes[hovered].id;
}

static auto consume(Vector<STRING::Cold> &pending, STRING::Hot id) -> Flag {
  const auto found = std::find(pending.begin(), pending.end(), id);
  if (found == pending.end()) return false;
  pending.erase(found);
  return true;
}

auto GUI::GET::pressed(Handle document) -> Flag {
  if (!live(document)) return false;
  return STATE::documents[document].pointer.landed;
}

auto GUI::GET::clicked(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.clicks, id);
}

auto GUI::GET::selected(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.selections, id);
}

auto GUI::GET::activated(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.activations, id);
}

auto GUI::GET::edited(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.edits, id);
}

auto GUI::GET::committed(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.commits, id);
}

auto GUI::GET::dialled(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.turns, id);
}

auto GUI::GET::moved(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  return ::consume(STATE::documents[document].pointer.moves, id);
}

auto GUI::GET::polls() -> Whole { return STATE::polls; }

auto GUI::GET::claimed() -> Handle { return STATE::claimed; }

auto GUI::GET::events(Handle document) -> const Vector<Event> & {
  static const Vector<Event> nothing;
  if (!live(document)) return nothing;
  return STATE::documents[document].pointer.events;
}
