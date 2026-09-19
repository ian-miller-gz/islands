import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "A login interpreter round-trips a raw marker over a pty (CONSOLES::PTY::)"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'pty.out')

run = Test.AddTestRun('round-trip')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
