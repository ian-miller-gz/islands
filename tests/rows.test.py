import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The SAC row stack: depths in, seated rails out (GUI::SAC::ROWS::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('rows')
run.Processes.Default.Command = f'{fixture} rows'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'rows.gold')
