import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Store: settings, records, and query tiers round-trip, persist, and refuse (STORE::)"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'store.out')

run = Test.AddTestRun('store')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
