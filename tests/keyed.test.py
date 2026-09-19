import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The unclaimed key: the drained readback no widget took (GUI::GET::keyed)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('keyed')
run.Processes.Default.Command = f'{fixture} keyed'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'keyed.gold')
