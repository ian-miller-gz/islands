import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "View: the integer and letterbox fits across window sizes (GFX::VIEW)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'view.out')

run = Test.AddTestRun('fits')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'view.gold')
