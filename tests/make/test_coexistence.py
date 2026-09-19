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


def test_token_set_renders_primary_and_mask() -> None:
  lines = Preprocessor._token("SR_NETWORK_BACKEND", ["SR_UNIX", "SR_TCP"])
  assert "#define SR_NETWORK_BACKEND SR_UNIX" in lines
  assert ("#define SR_NETWORK_BACKEND_SET ((1 << SR_UNIX) | (1 << SR_TCP))"
          in lines)


def test_scalar_token_renders_no_mask() -> None:
  lines = Preprocessor._token("SR_NETWORK_BACKEND", "SR_UNIX")
  assert lines == "#define SR_NETWORK_BACKEND SR_UNIX"


def _initializer(selected) -> Initializer:
  init = Initializer.__new__(Initializer)
  init._project_cfg = {
    "tokens": {"SR_NETWORK_BACKEND": selected},
    "variants": [
      {"selector": "SR_NETWORK_BACKEND",
       "options": {
         "network/backend/posix/unix": "SR_UNIX",
         "network/backend/posix/inet": ["SR_TCP", "SR_TCP6", "SR_SCTP"],
         "network/backend/posix/vsock": "SR_VSOCK"}}],
  }
  return init


def test_set_activates_every_member_subtree(tmp_path) -> None:
  init = _initializer(["SR_UNIX", "SR_TCP"])
  base = tmp_path / "network" / "backend" / "posix"
  assert init._is_active_variant(base / "unix")
  assert init._is_active_variant(base / "inet")
  assert not init._is_active_variant(base / "vsock")


def test_scalar_still_selects_one_subtree(tmp_path) -> None:
  init = _initializer("SR_UNIX")
  base = tmp_path / "network" / "backend" / "posix"
  assert init._is_active_variant(base / "unix")
  assert not init._is_active_variant(base / "inet")


def test_manifest_axis_accepts_the_list_form() -> None:
  axis = {"token": "SR_NETWORK_BACKEND",
          "values": {"unix": "SR_UNIX", "tcp": "SR_TCP"}}
  assert Initializer._spelled(axis, ["unix", "tcp"]) == ["SR_UNIX", "SR_TCP"]
  assert Initializer._spelled(axis, "tcp") == "SR_TCP"
  assert Initializer._spelled(axis, ["unix", "carrier"]) is None
  assert Initializer._spelled(axis, []) is None


def test_configuration_accepts_and_validates_the_list_form() -> None:
  config = {
    "tokens": {"network": {"SR_NETWORK_BACKEND": "SR_UNIX"}},
    "configurations": {
      "coexist": {"SR_NETWORK_BACKEND": ["SR_UNIX", "SR_TCP"]},
      "strange": {"SR_NETWORK_BACKEND": ["SR_UNIX", "SR_CARRIER"]}},
    "manifest": {"axes": {
      "network": {"token": "SR_NETWORK_BACKEND",
                  "values": {"unix": "SR_UNIX", "tcp": "SR_TCP"}}}},
  }
  init = Initializer.__new__(Initializer)
  init.configuration = "coexist"
  init.errored = False
  init._configure(config)
  assert config["tokens"]["network"]["SR_NETWORK_BACKEND"] == [
    "SR_UNIX", "SR_TCP"]
  assert not init.errored
  strange = Initializer.__new__(Initializer)
  strange.configuration = "strange"
  strange.errored = False
  strange._configure(config)
  assert strange.errored
