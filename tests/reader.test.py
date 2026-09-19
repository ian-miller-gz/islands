import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "A granted reader is answered over a bound TLS lane (NETWORK::WEB::SERVER::)"

arm = ('serve' if islands.token(Test.TestDirectory, 'SR_SERVER') == 'SR_NATIVE'
       else 'dark')
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'reader.out')
driver = os.path.join(Test.TestDirectory, 'reader.py')

run = Test.AddTestRun(arm)
run.Processes.Default.Command = f'python3 {driver} {fixture} {arm}'
run.Processes.Default.ReturnCode = 0
