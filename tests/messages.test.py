import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.listening(Test, Condition)
islands.dynamic(Test, Condition)
islands.transported(Test, Condition)

Test.Summary = "A listening engine publishes frame telemetry to the broker"

driver = os.path.join(Test.TestDirectory, 'messages.py')
run = Test.AddTestRun('frames')
run.Processes.Default.Command = (
  f'python3 {driver} {islands.binary(Test.TestDirectory)} '
  f'{islands.flags(frames=0)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'messages.gold')
