import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)

Test.Summary = "The island host restarts once, then terminates cleanly"

run = Test.AddTestRun("restart")
run.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} {islands.flags(restarts=1)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'restart.gold')
