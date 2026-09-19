import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI board: board space, the view, the carry (GUI::NGA::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('graph')
run.Processes.Default.Command = f'{fixture} graph'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'graph.gold')
