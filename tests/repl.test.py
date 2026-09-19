import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
islands.debug(Test, Condition)

Test.Summary = "Piped stdin commands respond on stdout and can end the run"

run = Test.AddTestRun("repl")
run.Processes.Default.Command = (
  f'printf "help\\nmetrics\\nquit\\n" | '
  f'{islands.binary(Test.TestDirectory)} {islands.flags(frames=0)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'repl.gold')
