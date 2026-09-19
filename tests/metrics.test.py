import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Metrics: counters/gauges/Scope, per-pass fold, names (METRICS::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'metrics.out')

run = Test.AddTestRun('surface')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
if islands.token(Test.TestDirectory, 'SR_METRICS', 'SR_REGISTRY') != 'SR_NONE':
  run.Processes.Default.Streams.stdout = islands.gold(
    Test.TestDirectory, config, 'metrics.gold')
