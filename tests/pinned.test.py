import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "A board child pinned on one axis holds it in face units"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('pinned')
run.Processes.Default.Command = f'{fixture} pinned'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'pinned.gold')
