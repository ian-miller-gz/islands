import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The SAC seat: a row's id and the row read back (GUI::SAC::SEAT::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('seat')
run.Processes.Default.Command = f'{fixture} seat'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'seat.gold')
