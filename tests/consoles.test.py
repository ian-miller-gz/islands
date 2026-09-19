import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Console instances: guests, the linked line shell as a mode, two children"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'consoles.out')

run = Test.AddTestRun('instances')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
