import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's third query: what a close orphans (GUI::NGA::GET::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('orphans')
run.Processes.Default.Command = f'{fixture} orphans'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'orphans.gold')
