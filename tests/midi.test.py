import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "MIDI inputs: the wire decodes, the ports merge, and the sweep hands them back"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'midi.out')

run = Test.AddTestRun('midi')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
