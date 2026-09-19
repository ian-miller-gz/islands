import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI pool: the tail reclaimed, the hidden skipped"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('pool')
run.Processes.Default.Command = f'{fixture} pool'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'pool.gold')
