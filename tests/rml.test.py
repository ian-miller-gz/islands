import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The RmlUi translator pair: emit and import (GUI::TRANSLATE::rml)"

Test.SkipIf(Condition(
  lambda: islands.token(
    Test.TestDirectory, 'SR_GUI_BACKEND', 'SR_NATIVE') != 'SR_RMLUI',
  'the RmlUi translator is not compiled (SR_GUI_BACKEND: SR_NATIVE)'))

config = islands.backend(Test.TestDirectory)
skin = islands.skin(Test.TestDirectory, config)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')
fixtures = os.path.join(islands.root(Test.TestDirectory), 'tests', 'fixtures', 'rml')

cycle = Test.AddTestRun('round')
cycle.Processes.Default.Command = (
  f'{fixture} rml {os.path.join(fixtures, "round.gui")}')
cycle.Processes.Default.ReturnCode = 0
cycle.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, skin, 'rml.round.gold')

pair = Test.AddTestRun('pair')
pair.Processes.Default.Command = (
  f'{fixture} import {os.path.join(fixtures, "pair.rml")}')
pair.Processes.Default.ReturnCode = 0
pair.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, skin, 'rml.pair.gold')
