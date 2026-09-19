import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "A board child clipped away at a point answers no press there"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('phantom')
run.Processes.Default.Command = f'{fixture} phantom'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'phantom.gold')
