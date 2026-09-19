import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The board saved: the link line and the words it draws (GUI::NGA::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('saved')
run.Processes.Default.Command = f'{fixture} saved'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'saved.gold')
