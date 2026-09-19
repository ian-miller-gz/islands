import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The engine file dialog: open/save path collection (GUI::DIALOG)"

Test.SkipIf(Condition(
  lambda: islands.token(
    Test.TestDirectory, 'SR_GUI_BACKEND', 'SR_NATIVE') != 'SR_NATIVE',
  'the dialog gold is pinned on the native GUI backend only'))

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

sheet = os.path.join(
  islands.root(Test.TestDirectory), 'assets', 'islands.gui')

run = Test.AddTestRun('dialog')
run.Processes.Default.Command = (
  f'mkdir -p assets && cp {sheet} assets/ && {fixture} dialog')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'dialog.gold')
