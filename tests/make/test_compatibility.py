__copyright__ = """
 ===========================================================================
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
 ============================================================================"""

from make.initializer import Initializer
from make.preprocessor import Preprocessor


def _axes() -> dict:
  return {
    "platform": {"token": "SR_PLATFORM",
                 "values": {"posix": "SR_POSIX", "web": "SR_WEB"}},
    "network": {"token": "SR_NETWORK_BACKEND",
                "values": {"unix": "SR_UNIX", "tcp": "SR_TCP",
                           "none": "SR_NONE"}},
    "listen": {"token": "SR_LISTEN",
               "values": {"unix": "SR_UNIX", "none": "SR_NONE"}},
  }


def _config(tokens: dict) -> dict:
  return {
    "tokens": {"group": tokens},
    "manifest": {
      "axes": _axes(),
      "compatibility": {
        "platform": {"web": {"admits": {"network": ["none"],
                                        "listen": ["none"]}}},
        "network": {"none": {"admits": {"listen": ["none"]}}},
      },
    },
  }


def _initializer() -> Initializer:
  init = Initializer.__new__(Initializer)
  init.errored = False
  return init


def test_vet_passes_a_legal_composition() -> None:
  config = _config({"SR_PLATFORM": "SR_WEB", "SR_NETWORK_BACKEND": "SR_NONE",
                    "SR_LISTEN": "SR_NONE"})
  init = _initializer()
  init._vet(config)
  assert not init.errored


def test_vet_refuses_a_whitelisted_pairing() -> None:
  config = _config({"SR_PLATFORM": "SR_WEB", "SR_NETWORK_BACKEND": "SR_NONE",
                    "SR_LISTEN": "SR_UNIX"})
  init = _initializer()
  init._origin("SR_LISTEN", "meadow")
  init._vet(config)
  assert init.errored


def test_vet_ignores_an_uncarried_setting() -> None:
  config = _config({"SR_PLATFORM": "SR_POSIX", "SR_NETWORK_BACKEND": "SR_UNIX",
                    "SR_LISTEN": "SR_UNIX"})
  init = _initializer()
  init._vet(config)
  assert not init.errored


def test_vet_reads_a_set_member_against_the_whitelist() -> None:
  config = _config({"SR_PLATFORM": "SR_WEB",
                    "SR_NETWORK_BACKEND": ["SR_UNIX", "SR_TCP"],
                    "SR_LISTEN": "SR_NONE"})
  init = _initializer()
  init._vet(config)
  assert init.errored


def test_vet_refuses_over_a_blacklist() -> None:
  config = _config({"SR_PLATFORM": "SR_WEB", "SR_NETWORK_BACKEND": "SR_NONE",
                    "SR_LISTEN": "SR_NONE"})
  config["manifest"]["compatibility"] = {
    "network": {"none": {"refuses": {"listen": ["none"]}}}}
  init = _initializer()
  init._vet(config)
  assert init.errored


def test_the_zero_tier_is_exclusive_in_a_configuration_set() -> None:
  config = _config({"SR_NETWORK_BACKEND": "SR_UNIX"})
  config["configurations"] = {
    "broken": {"SR_NETWORK_BACKEND": ["SR_UNIX", "SR_NONE"]}}
  init = _initializer()
  init.configuration = "broken"
  init._configure(config)
  assert init.errored


def test_configure_records_the_cell_as_the_origin() -> None:
  config = _config({"SR_LISTEN": "SR_UNIX"})
  config["configurations"] = {"cell": {"SR_LISTEN": "SR_NONE"}}
  init = _initializer()
  init.configuration = "cell"
  init._configure(config)
  assert init.origins["SR_LISTEN"] == "configuration 'cell'"


def test_the_table_renders_sentinel_free_rows() -> None:
  rows = Preprocessor._compatibilities(
    {"manifest": {"compatibility": {
      "platform": {"web": {"admits": {"listen": ["none"]}}}}}})
  assert rows[0] == "#define SR_COMPATIBILITIES \\"
  assert rows[1] == '  {"platform", "web", "listen", "none", ""},'
  assert Preprocessor._compatibilities({}) == ["#define SR_COMPATIBILITIES"]
