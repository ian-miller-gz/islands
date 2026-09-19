import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "One binary connects a unix session and a coexisting secondary"

Test.SkipIf(Condition(
  lambda: not (islands.carries(Test.TestDirectory, 'SR_UNIX')
               and (islands.carries(Test.TestDirectory, 'SR_TCP')
                    or islands.carries(Test.TestDirectory, 'SR_TLS'))),
  'the built transport set does not carry a coexisting stream pair'))

secondary = 'tcp' if islands.carries(Test.TestDirectory, 'SR_TCP') else 'tls'
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'streams.out')
driver = os.path.join(Test.TestDirectory, 'streams.py')

run = Test.AddTestRun('coexist')
run.Processes.Default.Command = f'python3 {driver} {fixture} {secondary}'
run.Processes.Default.ReturnCode = 0
