import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The raw-key encoder maps key events to interpreter bytes (TERMINAL::KEYS::encode)"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'keys.out')

run = Test.AddTestRun('encode')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
