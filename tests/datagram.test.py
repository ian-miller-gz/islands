import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Two loopback UDP endpoints round-trip a datagram (NETWORK::DATAGRAM::)"

Test.SkipIf(Condition(
  lambda: islands.token(Test.TestDirectory, 'SR_DATAGRAM_BACKEND') == 'SR_NONE',
  'the binary compiles no datagram surface (SR_DATAGRAM_BACKEND: SR_NONE)'))

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'datagram.out')

run = Test.AddTestRun('round-trip')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
