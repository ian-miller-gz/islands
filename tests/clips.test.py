import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Audio clips: the buffer overload plays from memory and the WAV header is honored"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'clips.out')

run = Test.AddTestRun('clips')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
