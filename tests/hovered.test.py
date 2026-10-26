import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's resting pointer: hover state, in board units (GUI::NGA::)"

config = islands.backend(Test.TestDirectory)
islands.native(Test, Condition)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('hovered')
run.Processes.Default.Command = f'{fixture} hovered'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'hovered.gold')
