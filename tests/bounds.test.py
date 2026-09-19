import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's declared walls: the settled pan, at every writer (GUI::NGA::)"

config = islands.backend(Test.TestDirectory)
islands.native(Test, Condition)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('bounds')
run.Processes.Default.Command = f'{fixture} bounds'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'bounds.gold')
