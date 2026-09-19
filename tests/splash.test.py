import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands
import driver

config = islands.backend(Test.TestDirectory)
islands.listening(Test, Condition)
islands.dynamic(Test, Condition)
islands.native(Test, Condition)
islands.debug(Test, Condition)

Test.Summary = "The engine-drawn splash controls compose and close the run"

binary = islands.binary(Test.TestDirectory)
client = os.path.join(Test.TestDirectory, 'paced.py')
bundle = 'tests/fixtures/splash'
root = islands.root(Test.TestDirectory)
Test.SkipIf(Condition(
  lambda: not os.path.exists(os.path.join(root, bundle, 'posix',
                                          'libdemo.so')),
  'the splash fixture is missing (python3 -m make)'))

run = Test.AddTestRun('chrome')
sock = driver.endpoint()
run.Processes.Default.Command = (
  f'{binary} {islands.flags(frames=0, cartridge=bundle)} '
  f'--listen {sock} > splash.engine.out 2>&1 & '
  f'python3 {client} {sock} "target islands.splash" "read swell" '
  f'"read leave" "click leave" || kill $$!; wait $$!')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 120
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'splash.gold')
