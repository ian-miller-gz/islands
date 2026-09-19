import sys
sys.path.insert(0, Test.TestDirectory)
import islands

islands.release(Test, Condition)

Test.Summary = "The drive vocabulary is stripped from a release binary"

run = Test.AddTestRun('stripped')
run.Processes.Default.Command = (
  f'printf "click x\\nquit\\n" | '
  f'{islands.binary(Test.TestDirectory)} {islands.flags(frames=0)} '
  f'2>/dev/null | grep -F "Unknown command: click"')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 60
