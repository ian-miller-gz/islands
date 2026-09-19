import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Threads: hot workers, nested stop, Signal handoffs (THREADS::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'threads.out')

run = Test.AddTestRun('relay')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'threads.gold')
