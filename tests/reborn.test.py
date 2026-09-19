import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The document's build count across a theme rebuild (GUI::GET::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('reborn')
run.Processes.Default.Command = f'{fixture} reborn'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'reborn.gold')
