import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)

Test.Summary = "The reach words transfer, size the window, and arm a capture"

run = Test.AddTestRun("reach")
run.Processes.Default.Command = (
  f'printf "web\\nweb https://api.example.com/data\\nwindow\\n'
  f'window size 800x600\\nwindow\\nwindow fullscreen on\\n'
  f'screenshot temp/reach.ppm\\nquit\\n" | '
  f'{islands.binary(Test.TestDirectory)} '
  f'{islands.flags(frames=0, width=480, height=320)}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'reach.gold')
