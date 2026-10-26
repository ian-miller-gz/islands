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

config = islands.backend(Test.TestDirectory)
modal = Test.AddTestRun('modal')
modal.Processes.Default.Command = f'{fixture} modal'
modal.Processes.Default.ReturnCode = 0
modal.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'gui.modal.gold')
