import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
islands.debug(Test, Condition)
islands.benched(Test, Condition)

Test.Summary = "version reports the compiled configuration, one line per axis"

run = Test.AddTestRun("version")
run.Processes.Default.Command = (
  f'printf "version\\nquit\\n" | '
  f'{islands.binary(Test.TestDirectory)} {islands.flags(frames=0)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'version.gold')
