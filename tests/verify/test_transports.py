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
import re
import subprocess

import pytest

from verify.diagnostics import command
from env import ROOT_DIRECTORY

GENERATED = ROOT_DIRECTORY / "build" / "include" / "generated" / "network.hpp"
BACKEND = ROOT_DIRECTORY / "src" / "network" / "backend" / "posix"
SELECTIONS = (
    ("SR_ABSTRACT", "abstract"),
    ("SR_TCP", "inet"),
    ("SR_TCP6", "inet"),
    ("SR_SCTP", "inet"),
    ("SR_VSOCK", "vsock"),
    ("SR_TLS", "tls"),
)


@pytest.fixture(scope="module")
def head() -> list[str]:
    return command()


@pytest.mark.parametrize("token,subtree", SELECTIONS)
def test_transport_selection_compiles(head, tmp_path, token, subtree) -> None:
    if not GENERATED.exists():
        pytest.skip("no generated header yet (python3 -m make)")
    (tmp_path / "generated").mkdir()
    text = re.sub(r"#define SR_NETWORK_BACKEND \w+",
                  f"#define SR_NETWORK_BACKEND {token}", GENERATED.read_text())
    (tmp_path / "generated" / "network.hpp").write_text(text)
    shadowed = [head[0], "-I", str(tmp_path)] + head[1:]
    for source in sorted((BACKEND / subtree).glob("*.cpp")):
        run = subprocess.run(shadowed + [str(source)], cwd=ROOT_DIRECTORY,
                             capture_output=True, text=True)
        assert run.returncode == 0, f"{token} {source.name}:\n{run.stderr}"
