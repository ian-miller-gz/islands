import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The carries: one shape towed in the poll's own instant, and a grip's plate worn by the pointer"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('carry')
run.Processes.Default.Command = f'{fixture} carry'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'carry.gold')
