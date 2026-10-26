import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "A websocket opened, spoken and closed on 127.0.0.1 (NETWORK::WEB::SOCKET::)"

Test.SkipIf(Condition(
  lambda: islands.token(Test.TestDirectory, 'SR_WEB_BACKEND') != 'SR_NATIVE',
  'the binary compiles no web backend (SR_WEB_BACKEND is not SR_NATIVE)'))

fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'socket.out')
driver = os.path.join(Test.TestDirectory, 'socket.origin.py')

run = Test.AddTestRun('socket')
run.Processes.Default.Command = f'python3 {driver} {fixture}'
run.Processes.Default.ReturnCode = 0
