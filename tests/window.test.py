import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = (
  "The SAC window: the board's span in, its pans out (GUI::SAC::WINDOW::)")

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('window')
run.Processes.Default.Command = f'{fixture} window'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'window.gold')
