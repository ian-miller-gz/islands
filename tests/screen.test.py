import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The VT emulator turns canned escape sequences into the right grid"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'screen.out')

run = Test.AddTestRun('grid')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
