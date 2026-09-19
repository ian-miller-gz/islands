import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The wire's crossing column: halfway, stepped by the ports' ranks"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('route')
run.Processes.Default.Command = f'{fixture} route'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'route.gold')
