import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Audio streams: per-stream PCM streams mix, pace by frames, and sweep on remove"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'output.out')

run = Test.AddTestRun('streams')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
