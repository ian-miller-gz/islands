import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's wheel: the drained turn, board units, the list first (GUI::NGA::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('wheel')
run.Processes.Default.Command = f'{fixture} wheel'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'wheel.gold')
