import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Select: cycle, pick by index/name, and the marked listing (SELECT::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'select.out')

run = Test.AddTestRun('relay')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'select.gold')
