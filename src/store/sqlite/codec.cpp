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

#include <store/sqlite/records.internal.hpp>

namespace {

constexpr STRING::Buffer<6> TAGS = "biwfs";

void escape(String &out, const String &token) {
  for (char byte : token)
    if (byte == '\\')
      out += "\\\\";
    else if (byte == '\t')
      out += "\\t";
    else if (byte == '\n')
      out += "\\n";
    else
      out += byte;
}

auto unescape(const String &token) -> String {
  String out;
  for (Whole at = 0; at < token.size(); ++at) {
    if (token[at] != '\\' || at + 1 == token.size()) {
      out += token[at];
      continue;
    }
    char next = token[++at];
    out += next == 't' ? '\t' : next == 'n' ? '\n' : next;
  }
  return out;
}

auto split(const String &data) -> Vector<String> {
  Vector<String> tokens;
  Whole start = 0;
  for (Whole at = 0; at <= data.size(); ++at)
    if (at == data.size() || data[at] == '\t') {
      tokens.push_back(data.substr(start, at - start));
      start = at + 1;
    }
  return tokens;
}

void append(String &out, const FIELDS::Value &value) {
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

auto parse(char tag, const String &raw, FIELDS::Value &out) -> Flag {
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

auto STORE::RECORDS::encode(const FIELDS::Map &record) -> String {
  String out;
  for (const auto &[name, value] : record) {
    if (!out.empty()) out += '\t';
    escape(out, name);
    out += '\t';
    out += TAGS[value.index()];
    out += '\t';
    append(out, value);
  }
  return out;
}

auto STORE::RECORDS::decode(const String &blob) -> FIELDS::Map {
  FIELDS::Map record;
  Vector<String> tokens = split(blob);
  for (Whole at = 0; at + 2 < tokens.size(); at += 3)
    if (FIELDS::Value value;
        !tokens[at + 1].empty() &&
        parse(tokens[at + 1][0], unescape(tokens[at + 2]), value))
      record[unescape(tokens[at])] = value;
  return record;
}
