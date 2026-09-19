import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "Daemon relations: an owned child is spawned, swept, ended and reaped, and the close walk clears the graph"

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'relations.out')

run = Test.AddTestRun('relations')
run.Processes.Default.Command = fixture
run.Processes.Default.ReturnCode = 0
