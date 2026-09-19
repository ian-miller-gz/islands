
import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = 'TOPICS::DURABLE:: durable schedules survive a restart and re-arm'

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'durable.out')
home = os.path.join(Test.RunDirectory, 'durable.home')

filing = Test.AddTestRun('file')
filing.Processes.Default.Command = f'{fixture} {home} file'
filing.Processes.Default.ReturnCode = 0

delivering = Test.AddTestRun('deliver')
delivering.Processes.Default.Command = f'{fixture} {home} deliver'
delivering.Processes.Default.ReturnCode = 0

checker = os.path.join(Test.TestDirectory, 'clean.py')
hosts = Test.AddTestRun('hosts')
hosts.Processes.Default.Command = (
  f'python3 {checker} durable {islands.root(Test.TestDirectory)}')
hosts.Processes.Default.ReturnCode = 0
