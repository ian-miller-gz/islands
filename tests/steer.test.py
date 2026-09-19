import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI list: key navigation, the focus claim, the wheel"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('steer')
run.Processes.Default.Command = f'{fixture} steer'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'steer.gold')
