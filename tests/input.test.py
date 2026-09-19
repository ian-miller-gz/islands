import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Audio streams: capture streams arrive, drain by frames, and sweep the microphone back"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'input.out')

run = Test.AddTestRun('streams')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
