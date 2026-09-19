import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Clock: first-pass zero, one stamp per pass, monotonic (CLOCK::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'clock.out')

run = Test.AddTestRun('relay')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'clock.gold')
