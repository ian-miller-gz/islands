import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The engine file dialog browses (GUI::DIALOG over the list kind)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

sheet = os.path.join(islands.root(Test.TestDirectory), 'assets', 'islands.gui')

run = Test.AddTestRun('browse')
run.Processes.Default.Command = (
  f'mkdir -p assets && cp {sheet} assets/ && {fixture} browse')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'browse.gold')
