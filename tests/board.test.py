import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's selection: click, marquee, keynav, view drag"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('board')
run.Processes.Default.Command = f'{fixture} select'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'board.gold')
