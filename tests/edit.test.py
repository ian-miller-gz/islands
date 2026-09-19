import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI field: typing, the caret, the seat (GUI::edit)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('edit')
run.Processes.Default.Command = f'{fixture} edit'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'edit.gold')
