import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board's text: the label rides the zoom (GUI::NGA::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('letters')
run.Processes.Default.Command = f'{fixture} letters'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'letters.gold')
