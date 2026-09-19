import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The SAC ladder: rungs in, dressed marks out (GUI::SAC::LADDER::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('ladder')
run.Processes.Default.Command = f'{fixture} ladder'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'ladder.gold')
