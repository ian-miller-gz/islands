import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The window wears a picture as its pointer (WINDOW::wear / GET::worn)"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'worn.out')

run = Test.AddTestRun('wear')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
