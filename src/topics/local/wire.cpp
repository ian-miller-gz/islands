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
#include <cstdlib>
#include <string>

#include <topics/local/local.internal.hpp>

namespace {

void escape(String &out, const String &tok) {
  for (char byte : tok)
    if (byte == '\\')
      out += "\\\\";
    else if (byte == '\t')
      out += "\\t";
    else if (byte == '\n')
      out += "\\n";
    else
      out += byte;
}

auto unescape(const String &tok) -> String {
  String out;
  for (Whole at = 0; at < tok.size(); ++at) {
    if (tok[at] != '\\' || at + 1 == tok.size()) {
      out += tok[at];
      continue;
    }
    char next = tok[++at];
    out += next == 't' ? '\t' : next == 'n' ? '\n' : next;
  }
  return out;
}

auto split(const String &line) -> Vector<String> {
  Vector<String> tokens;
  Whole start = 0;
  for (Whole at = 0; at <= line.size(); ++at)
    if (at == line.size() || line[at] == '\t') {
      tokens.push_back(line.substr(start, at - start));
      start = at + 1;
    }
  return tokens;
}

void field(String &out, const String &name, const FIELDS::Value &value) {
  static constexpr STRING::Buffer<6> TAGS = "biwfs";
  out += '\t';
  escape(out, name);
  out += '\t';
  out += TAGS[value.index()];
  out += '\t';
  switch (value.index()) {
    case 0:
      out += std::get<Flag>(value) ? "1" : "0";
      break;
    case 1:
      escape(out, std::to_string(std::get<Integer>(value)));
      break;
    case 2:
      escape(out, std::to_string(std::get<Whole>(value)));
      break;
    case 3:
      escape(out, std::to_string(std::get<Float>(value)));
      break;
    case 4:
      escape(out, std::get<String>(value));
      break;
  }
}

auto value(char tag, const String &raw, FIELDS::Value &out) -> Flag {
  switch (tag) {
    case 'b':
      out = Flag(raw == "1");
      return true;
    case 'i':
      out = Integer(std::strtol(raw.c_str(), nullptr, 10));
      return true;
    case 'w':
      out = Whole(std::strtoul(raw.c_str(), nullptr, 10));
      return true;
    case 'f':
      out = Float(std::strtof(raw.c_str(), nullptr));
      return true;
    case 's':
      out = raw;
      return true;
    default:
      return false;
  }
}

}  // namespace

auto TOPICS::WIRE::subscribe(STRING::Hot topic) -> String {
  String out = "S\t";
  escape(out, topic);
  out += '\n';
  return out;
}

auto TOPICS::WIRE::publish(const String &topic, const Message &message)
  -> String {
  String out = "P\t";
  escape(out, topic);
  for (const auto &[name, held] : message) field(out, name, held);
  out += '\n';
  return out;
}

auto TOPICS::WIRE::decode(const String &line) -> Frame {
  Frame frame;
  Vector<String> tokens = split(line);
  if (tokens.size() >= 2 && tokens[0] == "S") {
    frame.verb = Verb::SUBSCRIBE;
    frame.topic = unescape(tokens[1]);
  } else if (tokens.size() >= 2 && tokens[0] == "P") {
    frame.verb = Verb::PUBLISH;
    frame.topic = unescape(tokens[1]);
    for (Whole at = 2; at + 2 < tokens.size(); at += 3)
      if (FIELDS::Value held;
          !tokens[at + 1].empty() &&
          value(tokens[at + 1][0], unescape(tokens[at + 2]), held))
        frame.message[unescape(tokens[at])] = held;
  }
  return frame;
}
