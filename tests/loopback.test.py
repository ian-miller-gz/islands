import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "One path answered once over plain http on 127.0.0.1 (NETWORK::WEB::LOOPBACK::)"

Test.SkipIf(Condition(
  lambda: islands.token(Test.TestDirectory, 'SR_SERVER') != 'SR_NATIVE',
  'the binary compiles no listener (SR_SERVER: SR_NONE)'))

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'loopback.out')
driver = os.path.join(Test.TestDirectory, 'loopback.py')

run = Test.AddTestRun('loopback')
run.Processes.Default.Command = f'python3 {driver} {fixture}'
run.Processes.Default.ReturnCode = 0
