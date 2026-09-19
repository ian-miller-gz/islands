import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Tilemap: solidity, surface, scroll clamp, and the cull window (TILEMAP::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'tilemap.out')

run = Test.AddTestRun('relay')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'tilemap.gold')
