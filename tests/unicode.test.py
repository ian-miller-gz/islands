import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "UTF-8 runs decode to codepoints, malformed shapes to U+FFFD (UNICODE::decode)"

root = islands.root(Test.TestDirectory)
fixture = os.path.join(root, 'build', 'outputs', 'unicode.out')

run = Test.AddTestRun('decode')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
