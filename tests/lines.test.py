import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Framer splits a byte stream into lines (NETWORK::LINES::)"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'lines.out')

run = Test.AddTestRun('edges')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
