import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Shared-memory ring: records cross a process boundary in order, byte-exact, with edge backpressure"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'conduit.out')

run = Test.AddTestRun('conduit')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
