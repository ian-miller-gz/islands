
import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = 'TOPICS:: constellation broker/link, cross-instance delivery'

Test.SkipIf(
  Condition(
    lambda: (
      islands.token(Test.TestDirectory, 'SR_TOPICS', 'SR_LOCAL') == 'SR_NONE'
    ),
    'the binary compiles no topics bus (SR_TOPICS: SR_NONE)',
  )
)
islands.transported(Test, Condition)

root = islands.root(Test.TestDirectory)
fixture = os.path.join(root, 'build', 'outputs', 'topics.out')
driver = os.path.join(Test.TestDirectory, 'topics_wire.py')

run = Test.AddTestRun('constellation')
run.Processes.Default.Command = f'python3 {driver} {fixture}'
run.Processes.Default.ReturnCode = 0
