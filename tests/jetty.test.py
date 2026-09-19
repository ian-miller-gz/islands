import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
islands.transported(Test, Condition)

Test.Summary = "The proof host composes a headless subset and serves a bundle"

driver = os.path.join(Test.TestDirectory, 'jetty.py')
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'jetty.out')
run = Test.AddTestRun('messages')
run.Processes.Default.Command = f'python3 {driver} {fixture}'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'jetty.gold')
