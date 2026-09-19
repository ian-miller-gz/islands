import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI tip: the hover answer rises from poll() alone"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('tip')
run.Processes.Default.Command = f'{fixture} tip'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'tip.gold')
