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
from pathlib import Path as PyPath

from make.initializer import Initializer
from make.make import Make


def _config(platform: str = "SR_POSIX") -> dict:
    return {
        "tokens": {"SR_PLATFORM": platform, "SR_POSIX": 9, "SR_WEB": 17},
        "manifest": {
            "path": "manifest.yaml",
            "section": "engine",
            "folds": {"axis": "platform", "section": "platforms"},
            "axes": {
                "platform": {
                    "token": "SR_PLATFORM",
                    "values": {"posix": "SR_POSIX", "web": "SR_WEB"},
                },
            },
        },
    }


def _initializer(config: dict) -> Initializer:
    init = Initializer.__new__(Initializer)
    init._project_cfg = config
    return init


def _make(config: dict, root: PyPath, fold: str) -> Make:
    make = Make.__new__(Make)
    make._project_cfg = config
    make.root = root
    make.fold = fold
    return make


def test_the_fold_is_the_composed_platform_word() -> None:
    assert _initializer(_config())._fold() == "posix"
    assert _initializer(_config("SR_WEB"))._fold() == "web"


def test_no_folds_declaration_leaves_deliveries_flat() -> None:
    config = _config()
    del config["manifest"]["folds"]
    assert _initializer(config)._fold() == ""


def test_a_bundle_destination_takes_the_fold(tmp_path) -> None:
    bundle = tmp_path / "cartridges" / "proof"
    bundle.mkdir(parents=True)
    (bundle / "manifest.yaml").write_text("name: proof\n")
    make = _make(_config(), PyPath(tmp_path), "posix")
    assert make._home("cartridges/proof") == bundle / "posix"


def test_a_plain_destination_keeps_its_flat_seat(tmp_path) -> None:
    drop = tmp_path / "libs"
    drop.mkdir()
    make = _make(_config(), PyPath(tmp_path), "posix")
    assert make._home("libs") == drop


def test_an_unfolded_build_delivers_flat(tmp_path) -> None:
    bundle = tmp_path / "cartridges" / "proof"
    bundle.mkdir(parents=True)
    (bundle / "manifest.yaml").write_text("name: proof\n")
    make = _make(_config(), PyPath(tmp_path), "")
    assert make._home("cartridges/proof") == bundle


def test_the_artifact_lands_in_the_fold(tmp_path) -> None:
    bundle = tmp_path / "cartridges" / "proof"
    bundle.mkdir(parents=True)
    (bundle / "manifest.yaml").write_text("name: proof\n")
    source = tmp_path / "libproof.so"
    source.write_bytes(b"artifact")
    make = _make(_config(), PyPath(tmp_path), "posix")
    make._copy_artifact(source, "libproof.so", ["cartridges/proof"])
    assert (bundle / "posix" / "libproof.so").read_bytes() == b"artifact"
    assert not (bundle / "libproof.so").exists()


def _bundle(tmp_path, document: str) -> tuple[dict, Initializer]:
    home = tmp_path / "cartridges" / "proof"
    home.mkdir(parents=True)
    (home / "manifest.yaml").write_text(document)
    config = _config()
    config["tokens"].update({"SR_CARTRIDGE": "SR_PROOF", "SR_PROOF": 2})
    config["tokens"].update({"SR_OVERLAY_BACKEND": "SR_IMGUI", "SR_IMGUI": 3,
                             "SR_NONE": 1})
    config["manifest"]["axes"]["overlay"] = {
        "token": "SR_OVERLAY_BACKEND",
        "values": {"imgui": "SR_IMGUI", "none": "SR_NONE"},
    }
    config["statics"] = [
        {"selector": "SR_CARTRIDGE", "options": {"cartridges/proof": "SR_PROOF"}},
    ]
    init = Initializer.__new__(Initializer)
    init.root = PyPath(tmp_path)
    init.errored = False
    return config, init


NEUTRAL = """
name: proof
engine:
  platform: [posix, web]
  overlay: imgui
platforms:
  web:
    engine:
      overlay: none
"""


def test_the_fold_delta_composes_over_the_neutral_document(tmp_path) -> None:
    config, init = _bundle(tmp_path, NEUTRAL)
    init._overlay(config)
    assert config["tokens"]["SR_OVERLAY_BACKEND"] == "SR_IMGUI"
    assert config["tokens"]["SR_PLATFORM"] == "SR_POSIX"


def test_the_delta_overrides_its_own_fold(tmp_path) -> None:
    config, init = _bundle(tmp_path, NEUTRAL)
    config["tokens"]["SR_PLATFORM"] = "SR_WEB"
    init._overlay(config)
    assert config["tokens"]["SR_OVERLAY_BACKEND"] == "SR_NONE"
    assert config["tokens"]["SR_PLATFORM"] == "SR_WEB"


def test_a_set_takes_its_primary_when_the_build_chose_none(tmp_path) -> None:
    config, init = _bundle(tmp_path, NEUTRAL)
    del config["tokens"]["SR_PLATFORM"]
    init._overlay(config)
    assert config["tokens"]["SR_PLATFORM"] == "SR_POSIX"


def test_an_unknown_platform_refuses(tmp_path) -> None:
    config, init = _bundle(tmp_path, "name: proof\nplatforms:\n  posex:\n    engine:\n      overlay: none\n")
    init._overlay(config)
    assert init.errored


def test_maps_deep_merge_and_scalars_override() -> None:
    base = {"engine": {"overlay": "imgui", "listen": "unix"}, "listen": True}
    delta = {"engine": {"listen": "none"}, "listen": False}
    merged = Initializer._merge(base, delta)
    assert merged == {"engine": {"overlay": "imgui", "listen": "none"},
                      "listen": False}
    assert base["engine"]["listen"] == "unix"
