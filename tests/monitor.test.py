import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
islands.listening(Test, Condition)

Test.Summary = "The monitoring words report an instance's state and vocabulary"

run = Test.AddTestRun("monitor")
run.Processes.Default.Command = (
  f'printf "status\\nlog\\ntopics\\ntopics peek nope\\ninstances\\nquit\\n" | '
  f'{islands.binary(Test.TestDirectory)} {islands.flags(frames=0, listen=0)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'monitor.gold')
