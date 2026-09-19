import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The SAC playhead: the blade and its stripcap (GUI::SAC::PLAYHEAD::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('playhead')
run.Processes.Default.Command = f'{fixture} playhead'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'playhead.gold')
