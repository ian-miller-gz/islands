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
import getpass
from pathlib import Path

from verify.diagnostics import hygiene


def table(root) -> dict:
    return {
        "roots": [str(root)],
        "extensions": [".gold"],
        "forbidden": [{
            "name": "absolute path outside the repo",
            "regex": r"(^|[^`\w])/(home|tmp|usr|var|etc|opt|root|run|mnt|media)(/|$|\s)",
        }],
        "allowed_hosts": [
            r"^127\.",
            r"^localhost$",
            r"\.(test|example|invalid|localhost)$",
            r"(^|\.)example\.(com|net|org)$",
        ],
    }


def test_clean_gold_passes(tmp_path) -> None:
    (tmp_path / "clean.gold").write_text(
        "`` [info] ~/island/window: Window created 640x480\n"
        "web transfer refused: https://api.example.com/data\n"
        "refused: http://elsewhere.test is not in the declared allowlist\n"
        "Serving on 127.0.0.1\n"
        "Loading Cartridge... cartridges/.test/meadow\n", encoding="utf-8")
    assert hygiene(table(tmp_path)) is True


def test_absolute_path_fails(tmp_path, capsys) -> None:
    (tmp_path / "leak.gold").write_text(
        "ok line\nwrote /tmp/islands-abc.sock\n", encoding="utf-8")
    assert hygiene(table(tmp_path)) is False
    out = capsys.readouterr().out
    assert "leak.gold:2" in out and "absolute path" in out


def test_unreserved_host_fails(tmp_path, capsys) -> None:
    (tmp_path / "leak.gold").write_text(
        "fetched https://api.stripe.com/v1\npinged 192.168.1.7\n",
        encoding="utf-8")
    assert hygiene(table(tmp_path)) is False
    out = capsys.readouterr().out
    assert "api.stripe.com" in out and "192.168.1.7" in out


def test_baking_identity_fails(tmp_path, capsys) -> None:
    (tmp_path / "leak.gold").write_text(
        f"session for {getpass.getuser()} opened\n"
        f"cwd {Path.home()}/work\n", encoding="utf-8")
    assert hygiene(table(tmp_path)) is False
    out = capsys.readouterr().out
    assert "user name" in out and "home directory" in out


def test_pixel_golds_stay_out_by_extension(tmp_path) -> None:
    (tmp_path / "frame.ppm").write_text(
        f"P6 {Path.home()} /tmp/x\n", encoding="utf-8")
    assert hygiene(table(tmp_path)) is True
