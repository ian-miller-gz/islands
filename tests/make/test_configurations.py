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
import yaml

from env import ROOT_DIRECTORY
from make.config import PROJECT_CONFIG_PATH
from make.initializer import Initializer


def _config() -> dict:
  return {
    "tokens": {
      "gfx": {"SR_GFX_BACKEND": "SR_VULKAN"},
      "cartridge": {"SR_CARTRIDGE": "SR_NONE"},
    },
    "configurations": {
      "bench": {},
      "sdl": {"SR_GFX_BACKEND": "SR_SDL3"},
      "static-meadow": {"SR_CARTRIDGE": "SR_MEADOW"},
    },
    "manifest": {
      "axes": {
        "gfx": {"token": "SR_GFX_BACKEND",
                "values": {"vulkan": "SR_VULKAN", "sdl3": "SR_SDL3"}},
      },
      "keys": {
        "shell": {"token": "SR_SHELL",
                  "values": {True: "SR_POSIX", False: "SR_NONE"},
                  "absent": "SR_NONE"},
      },
    },
  }


def _initializer(name: str | None) -> Initializer:
  init = Initializer.__new__(Initializer)
  init.configuration = name
  init.errored = False
  return init


def test_overlay_lands_in_its_token_group() -> None:
  config = _config()
  init = _initializer("sdl")
  init._configure(config)
  assert config["tokens"]["gfx"]["SR_GFX_BACKEND"] == "SR_SDL3"
  assert not init.errored


def test_empty_overlay_is_the_bench() -> None:
  config = _config()
  init = _initializer("bench")
  init._configure(config)
  assert config["tokens"]["gfx"]["SR_GFX_BACKEND"] == "SR_VULKAN"
  assert not init.errored


def test_no_configuration_is_a_plain_build() -> None:
  config = _config()
  Initializer.__new__(Initializer)._configure(config)
  assert config["tokens"]["gfx"]["SR_GFX_BACKEND"] == "SR_VULKAN"


def test_unknown_name_refuses_the_build() -> None:
  init = _initializer("nope")
  init._configure(_config())
  assert init.errored


def test_unknown_token_refuses_the_build() -> None:
  config = _config()
  config["configurations"]["sdl"]["SR_BOGUS"] = "SR_SDL3"
  init = _initializer("sdl")
  init._configure(config)
  assert init.errored


def test_vocabulary_refuses_an_unknown_value() -> None:
  config = _config()
  config["configurations"]["sdl"]["SR_GFX_BACKEND"] = "SR_METAL"
  init = _initializer("sdl")
  init._configure(config)
  assert init.errored


def test_unvocabularied_token_accepts_any_value() -> None:
  config = _config()
  init = _initializer("static-meadow")
  init._configure(config)
  assert config["tokens"]["cartridge"]["SR_CARTRIDGE"] == "SR_MEADOW"
  assert not init.errored


def test_two_lane_keys_join_the_vocabulary() -> None:
  known = Initializer._spellings(_config())
  assert known["SR_SHELL"] == {"SR_POSIX", "SR_NONE"}
  assert known["SR_GFX_BACKEND"] == {"SR_VULKAN", "SR_SDL3"}


def _real() -> dict:
  with open(ROOT_DIRECTORY / PROJECT_CONFIG_PATH, encoding="utf-8") as file:
    return yaml.safe_load(file)


def test_windows_platform_refuses_a_foldless_transport(monkeypatch) -> None:
  config = _real()
  monkeypatch.setattr("make.initializer.ROOT_DIRECTORY", ROOT_DIRECTORY)
  init = _initializer("windows")
  init._configure(config)
  Initializer._assign(config, "SR_NETWORK_BACKEND", "SR_UNIX")
  init._vet(config)
  assert init.errored
