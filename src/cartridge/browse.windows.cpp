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
#include <cartridge/browse.internal.hpp>

#include <string>

#define NOMINMAX
#include <windows.h>
#include <shellapi.h>

namespace CARTRIDGE::BROWSE {
namespace {

using Wide = std::wstring;

constexpr const wchar_t *VERB = L"open";
constexpr const wchar_t *QUOTE = L"\"";
constexpr const wchar_t *UNQUOTABLE = L"\"\\";
constexpr int TERMINATED = -1;
constexpr INT_PTR STARTED = 32;

auto widened(STRING::Hot text) -> Wide {
  const int size =
    MultiByteToWideChar(CP_UTF8, 0, text, TERMINATED, nullptr, 0);
  if (size <= 0) return {};
  Wide wide(static_cast<Whole>(size), L'\0');
  MultiByteToWideChar(CP_UTF8, 0, text, TERMINATED, wide.data(), size);
  wide.pop_back();
  return wide;
}

auto shell(const Wide &file, const wchar_t *parameters) -> Flag {
  const auto answer = reinterpret_cast<INT_PTR>(ShellExecuteW(
    nullptr, VERB, file.c_str(), parameters, nullptr, SW_SHOWNORMAL));
  return answer > STARTED;
}

}  // namespace
}  // namespace CARTRIDGE::BROWSE

auto CARTRIDGE::BROWSE::open(STRING::Hot program, STRING::Hot url) -> Flag {
  const Wide address = widened(url);
  if (address.empty() || address.find_first_of(UNQUOTABLE) != Wide::npos)
    return false;
  const Wide argument = QUOTE + address + QUOTE;
  return shell(widened(program), argument.c_str());
}

auto CARTRIDGE::BROWSE::open(STRING::Hot url) -> Flag {
  const Wide address = widened(url);
  return !address.empty() && shell(address, nullptr);
}
