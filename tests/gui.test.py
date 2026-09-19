import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI tree: creation, mutation, dirty discipline (GUI::)"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('tree')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
