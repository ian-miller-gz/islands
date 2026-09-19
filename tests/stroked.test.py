import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's standing stroke: held state, board units"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('stroked')
run.Processes.Default.Command = f'{fixture} stroked'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'stroked.gold')
