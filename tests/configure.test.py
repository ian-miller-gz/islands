import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)

Test.Summary = "configure mirrors the runtime-preference lane and writes it"

run = Test.AddTestRun("configure")
run.Processes.Default.Command = (
  f'printf "configure\\nconfigure vsync 0\\nconfigure size 800x600\\n'
  f'configure logs warn\\nconfigure nonsense\\nquit\\n" | '
  f'{islands.binary(Test.TestDirectory)} {islands.flags(frames=0)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'configure.gold')
