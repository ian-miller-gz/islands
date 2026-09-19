import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands
import driver

config = islands.backend(Test.TestDirectory)
islands.listening(Test, Condition)
islands.transported(Test, Condition)

Test.SkipIf(Condition(
  lambda: islands.token(Test.TestDirectory, 'SR_WEB_BACKEND') != 'SR_NATIVE',
  'the binary compiles no web backend (SR_WEB_BACKEND is not SR_NATIVE)'))

Test.Summary = "A session transfers over ftp:// (NETWORK::WEB, SR_NATIVE)"

REACH = '"ftp to 127.0.0.1"'

client = os.path.join(Test.TestDirectory, 'ftp.py')
run = Test.AddTestRun('native')
sock = driver.endpoint()
run.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} '
  f'{islands.flags(frames=0, cartridge="none", web=REACH)} '
  f'--listen {sock} > ftp.engine.out 2>&1 & '
  f'python3 {client} {sock} || kill $$!; wait $$!')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'ftp.native.gold')
