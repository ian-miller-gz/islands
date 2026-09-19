import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "TOPICS:: publish/subscribe/drain, bounded backlog (src/topics)"

root = islands.root(Test.TestDirectory)
fixture = os.path.join(root, 'build', 'outputs', 'topics.out')

run = Test.AddTestRun('bus')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
