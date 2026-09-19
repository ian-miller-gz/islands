import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Scenes: named lookup, enter/update/render walk, null-hook safety (SCENES::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'scenes.out')

run = Test.AddTestRun('relay')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'scenes.gold')
