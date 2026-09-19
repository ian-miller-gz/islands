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
#include <island/shell/engine.internal.hpp>
#include <island/gui/gui.hpp>
#include <format>
#include <generated/profile.hpp>
#include <generated/values.hpp>

#if !defined(SR_PROFILE)
#error "SR_PROFILE is not defined; set it in configs/make.yaml (tokens:)."
#endif

#if SR_PROFILE == SR_DEBUG

namespace ISLAND::SHELL {
namespace {

namespace STATE {
GUI::Handle target = GUI::NONE;
}  // namespace STATE

constexpr STRING::Hot WORDS[] = {"enter",  "exit",     "press", "click",
                                 "select", "activate", "edit",  "commit",
                                 "dial",   "move",     "join",  "cut"};

static_assert(std::size(WORDS) == GUI::Event::CUT + 1);

auto resolve(::SHELL::Session &session) -> GUI::Handle {
  const auto document =
    STATE::target != GUI::NONE ? STATE::target : GUI::GET::document();
  if (document == GUI::NONE)
    session.print("no target: pick one with target <document>");
  return document;
}

auto tail(GUI::Handle document, Whole from) -> String {
  const auto &events = GUI::GET::events(document);
  String text;
  for (Whole index = from; index < events.size(); index += 1) {
    text += text.empty() ? "" : ",";
    text += String(WORDS[events[index].kind]) + ":" + String(events[index].id);
  }
  return text.empty() ? "-" : text;
}

struct Chord {
  STRING::Hot word;
  Flag INPUT::KEYS::Event::*raises;
};

constexpr Chord CHORDS[] = {
  {"ctrl", &INPUT::KEYS::Event::control},
  {"shift", &INPUT::KEYS::Event::shift},
  {"alt", &INPUT::KEYS::Event::alt}};

auto chorded(const String &word, INPUT::KEYS::Event &event) -> Flag {
  for (const auto &chord : CHORDS) {
    if (word != chord.word) continue;
    event.*chord.raises = true;
    return true;
  }
  return false;
}

auto said(const INPUT::KEYS::Event &event) -> String {
  String words;
  for (const auto &chord : CHORDS)
    if (event.*chord.raises) words += " " + String(chord.word);
  return words;
}

constexpr Whole ID = 2;
constexpr Whole PAIR = 3;

auto trailed(
  const Vector<String> &arguments, Whole floor,
  INPUT::KEYS::Event &chord) -> Whole {
  Whole count = arguments.size();
  while (count > floor && chorded(arguments[count - 1], chord)) count -= 1;
  return count;
}

auto held(const INPUT::KEYS::Event &chord) -> INPUT::Chord {
  return {chord.control, chord.shift, chord.alt};
}

void target(::SHELL::Session &session) {
  if (session.arguments.size() < 2) {
    const auto document =
      STATE::target != GUI::NONE ? STATE::target : GUI::GET::document();
    if (document == GUI::NONE)
      return session.print("no target: target <document>");
    return session.print(std::format("target document {}", document));
  }
  const auto found = GUI::GET::document(session.arguments[1].c_str());
  if (found == GUI::NONE)
    return session.print("no document matches: " + session.arguments[1]);
  STATE::target = found;
  session.print(
    std::format("target {} = document {}", session.arguments[1], found));
}

void click(::SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("usage: click <id>");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (GUI::click(document, id.c_str()) != 0)
    return session.print("click refused: " + id);
  const auto hover = GUI::GET::hover(document);
  session.print(
    "click " + id + " hover=" + (hover.empty() ? "-" : String(hover)) +
    " events=" + tail(document, from));
}

void doubled(::SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("usage: doubleclick <id>");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (GUI::doubleclick(document, id.c_str()) != 0)
    return session.print("doubleclick refused: " + id);
  session.print("doubleclick " + id + " events=" + tail(document, from));
}

void pressed(::SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("usage: press <id> [<x> <y>] [ctrl] [shift] [alt]");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  INPUT::KEYS::Event chord;
  const Whole count = trailed(session.arguments, ID, chord);
  const Float x =
    count > 3 ? STRING::number<Float>(session.arguments[2]) : 0.0f;
  const Float y =
    count > 3 ? STRING::number<Float>(session.arguments[3]) : 0.0f;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (GUI::press(document, id.c_str(), x, y, held(chord)) != 0)
    return session.print("press refused: " + id);
  session.print(
    "press " + id + said(chord) + " events=" + tail(document, from));
}

void asked(::SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("usage: ask <id> [<x> <y>]");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  const Float x = session.arguments.size() > 3
                    ? STRING::number<Float>(session.arguments[2])
                    : 0.0f;
  const Float y = session.arguments.size() > 3
                    ? STRING::number<Float>(session.arguments[3])
                    : 0.0f;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (GUI::ask(document, id.c_str(), x, y) != 0)
    return session.print("ask refused: " + id);
  session.print("ask " + id + " events=" + tail(document, from));
}

void travelled(::SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("usage: travel <x> <y>");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (
    GUI::travel(
      document, STRING::number<Float>(session.arguments[1]),
      STRING::number<Float>(session.arguments[2])) != 0)
    return session.print("travel refused: no standing press");
  session.print("travel events=" + tail(document, from));
}

void released(::SHELL::Session &session) {
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const Float x = session.arguments.size() > 2
                    ? STRING::number<Float>(session.arguments[1])
                    : 0.0f;
  const Float y = session.arguments.size() > 2
                    ? STRING::number<Float>(session.arguments[2])
                    : 0.0f;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (GUI::release(document, x, y) != 0)
    return session.print("release refused: no standing press");
  session.print("release events=" + tail(document, from));
}

void drag(::SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("usage: drag <from> <to> | drag <id> <x> <y> [holds]");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  INPUT::KEYS::Event chord;
  const Whole count = trailed(session.arguments, PAIR, chord);
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  Status refused = 0;
  if (count == 3)
    refused = GUI::drag(
      document, id.c_str(), session.arguments[2].c_str(), held(chord));
  else
    refused = GUI::drag(
      document, id.c_str(), STRING::number<Float>(session.arguments[2]),
      STRING::number<Float>(session.arguments[3]),
      count > 4 ? STRING::number<Whole>(session.arguments[4], 1) : 1,
      held(chord));
  if (refused != 0) return session.print("drag refused: " + id);
  session.print("drag " + id + said(chord) + " events=" + tail(document, from));
}

void wheel(::SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("usage: wheel <id> <turns> [control]");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  const Flag control =
    session.arguments.size() > 3 && session.arguments[3] == "control";
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  if (
    GUI::wheel(
      document, id.c_str(), STRING::number<Float>(session.arguments[2]),
      control) != 0)
    return session.print("wheel refused: " + id);
  session.print(std::format(
    "wheel {} turns={:g} control={} events={}", id,
    STRING::number<Float>(session.arguments[2]), control ? "true" : "false",
    tail(document, from)));
}

struct Stroke {
  STRING::Hot name;
  INPUT::KEYS::Action action;
  Whole codepoint = 0;
};

constexpr Whole CASED = 'a' - 'A';
constexpr Whole LETTER = 1;

constexpr Stroke STROKES[] = {{"enter", INPUT::KEYS::ENTER},
                              {"erase", INPUT::KEYS::ERASE},
                              {"delete", INPUT::KEYS::DELETE},
                              {"left", INPUT::KEYS::LEFT},
                              {"right", INPUT::KEYS::RIGHT},
                              {"up", INPUT::KEYS::UP},
                              {"down", INPUT::KEYS::DOWN},
                              {"home", INPUT::KEYS::HOME},
                              {"end", INPUT::KEYS::END},
                              {"prior", INPUT::KEYS::PRIOR},
                              {"next", INPUT::KEYS::NEXT},
                              {"tab", INPUT::KEYS::TEXT, INPUT::KEYS::TAB}};

auto composed(const String &name, INPUT::KEYS::Event &event) -> Flag {
  for (const auto &entry : STROKES) {
    if (name != entry.name) continue;
    event = {entry.action, entry.codepoint};
    return true;
  }
  if (name.size() != LETTER) return false;
  const Whole letter = static_cast<unsigned char>(name[0]);
  event = {
    INPUT::KEYS::TEXT,
    letter >= 'a' && letter <= 'z' ? letter - CASED : letter};
  return true;
}

void key(::SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("usage: key <name> [ctrl] [shift] [alt]");
  const auto &name = session.arguments[1];
  INPUT::KEYS::Event event;
  if (!composed(name, event)) return session.print("unknown key: " + name);
  for (Whole index = 2; index < session.arguments.size(); index += 1)
    if (!chorded(session.arguments[index], event))
      return session.print("unknown chord: " + session.arguments[index]);
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  GUI::key(document, event);
  session.print(
    "key " + name + said(event) + " events=" + tail(document, from));
}

void type(::SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("usage: type <text>");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  String text;
  for (Whole index = 1; index < session.arguments.size(); index += 1) {
    if (!text.empty()) text += ' ';
    text += session.arguments[index];
  }
  GUI::stage(document);
  const auto from = GUI::GET::events(document).size();
  for (const Char letter : text)
    GUI::key(document, {INPUT::KEYS::TEXT, static_cast<unsigned char>(letter)});
  session.print("type events=" + tail(document, from));
}

void read(::SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("usage: read <id>");
  const auto document = resolve(session);
  if (document == GUI::NONE) return;
  const auto &id = session.arguments[1];
  const auto corner = GUI::GET::origin(document, id.c_str());
  const auto size = GUI::GET::measured(document, id.c_str());
  const auto text = GUI::GET::text(document, id.c_str());
  session.print(std::format(
    "read {} rect={:g},{:g} {:g}x{:g} text={}", id, corner.x, corner.y, size.w,
    size.h, String(text)));
}

}  // namespace

auto debug() -> const Vector<::SHELL::Command> * {
  static const Vector<::SHELL::Command> table = {
    {"target", "aims the drive commands at a document", target},
    {"click", "synthesizes a press/release arc on a widget", click},
    {"doubleclick", "synthesizes two arcs inside the double window", doubled},
    {"press", "presses a point on a widget and holds it standing", pressed},
    {"release", "puts the standing press down, travelling first if told",
     released},
    {"travel", "moves the standing press by an offset without lifting it",
     travelled},
    {"ask", "the off-hand still press on a node or a board spot", asked},
    {"drag", "presses on a widget and pulls to another or an offset", drag},
    {"wheel", "turns the wheel over a widget, with or without control", wheel},
    {"key", "feeds one composed key to the target document", key},
    {"type", "types text into the target document", type},
    {"read", "reads a widget's compiled rect and text", read},
    {"dialog", "raises the engine file dialog for driving", dialog}};
  return &table;
}

}  // namespace ISLAND::SHELL

#else

auto ISLAND::SHELL::debug() -> const Vector<::SHELL::Command> * {
  return nullptr;
}

#endif
