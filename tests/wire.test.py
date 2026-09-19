import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The wiring gesture: pending, JOINED, assisted, CUT, cancel"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('wire')
run.Processes.Default.Command = f'{fixture} wire'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'wire.gold')
