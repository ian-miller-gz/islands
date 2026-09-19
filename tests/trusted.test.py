import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands
import driver

config = islands.backend(Test.TestDirectory)
islands.listening(Test, Condition)
islands.transported(Test, Condition)
islands.dynamic(Test, Condition)

Test.SkipIf(Condition(
  lambda: islands.token(Test.TestDirectory, 'SR_WEB_BACKEND') != 'SR_NATIVE',
  'the binary compiles no web backend (SR_WEB_BACKEND is not SR_NATIVE)'))

bundle = 'tests/fixtures/trusted'
root = islands.root(Test.TestDirectory)
Test.SkipIf(Condition(
  lambda: not os.path.exists(os.path.join(root, bundle, 'posix',
                                          'libdemo.so')),
  'the trusted fixture is missing (python3 -m make)'))

Test.Summary = "A manifest-declared certificate is trusted for its host alone"

client = os.path.join(Test.TestDirectory, 'trusted.py')
run = Test.AddTestRun('declared')
sock = driver.endpoint()
run.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} '
  f'{islands.flags(frames=0, cartridge=bundle)} '
  f'--listen {sock} > declared.engine.out 2>&1 & '
  f'python3 {client} {sock} || kill $$!; wait $$!')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'trusted.declared.gold')
