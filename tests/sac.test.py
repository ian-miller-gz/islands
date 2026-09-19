import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The SAC designation gate: undesignated refuses, adopt arms (GUI::SAC::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')
sheet = os.path.join(
  islands.root(Test.TestDirectory), 'assets', 'islands.gui')

run = Test.AddTestRun('sac')
run.Processes.Default.Command = (
  f'mkdir -p assets && cp {sheet} assets/ && {fixture} sac')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'sac.gold')
