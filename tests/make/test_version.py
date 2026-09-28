import pathlib

import pytest
import yaml

from make import version

ROOT = pathlib.Path(__file__).resolve().parents[2]


def test_tools_state_a_release() -> None:
  assert version.stated() != "0.0.0"


def test_no_requirement_constrains_nothing() -> None:
  version.require({})


def test_matching_requirement_passes() -> None:
  version.require({"tools": version.stated()})


def test_other_requirement_refuses_with_both_numbers() -> None:
  with pytest.raises(SystemExit) as refused:
    version.require({"tools": "9.9.9"})
  assert "9.9.9" in str(refused.value)
  assert version.stated() in str(refused.value)


def test_the_checkout_requires_its_own_pin() -> None:
  config = yaml.safe_load((ROOT / "configs" / "make.yaml").read_text())
  assert version.required(config) == version.stated()
