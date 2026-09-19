import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "STRING:: trim/split/number/stem and the key:value read (src/common/string)"

root = islands.root(Test.TestDirectory)
fixture = os.path.join(root, 'build', 'outputs', 'string.out')

run = Test.AddTestRun('utilities')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
