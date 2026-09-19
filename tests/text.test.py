import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Cartridge text stream: INPUT::TEXT is opt-in, idle by default, and yields on a claim"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'text.out')

run = Test.AddTestRun('stream')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
