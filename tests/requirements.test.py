import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
islands.transported(Test, Condition)

Test.Summary = "Cartridges declare requirements; the engine delivers them"

driver = os.path.join(Test.TestDirectory, 'requirements.py')
run = Test.AddTestRun('policies')
run.Processes.Default.Command = (
  f'python3 {driver} {islands.binary(Test.TestDirectory)} '
  f'{islands.flags(frames=0)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'requirements.gold')
