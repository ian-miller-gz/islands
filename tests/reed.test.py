import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The AUDIO::PLUGIN:: surface renders instances headless, deterministic and concurrent-safe"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'reed.out')

run = Test.AddTestRun('reed')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
