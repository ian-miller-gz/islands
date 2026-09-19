import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands
import driver

config = islands.backend(Test.TestDirectory)
islands.listening(Test, Condition)
islands.transported(Test, Condition)

Test.SkipIf(Condition(
  lambda: islands.token(Test.TestDirectory, 'SR_WEB_BACKEND') != 'SR_NONE',
  'the binary compiles a web backend (SR_WEB_BACKEND is not SR_NONE)'))

Test.Summary = "A socket session probes the web capability gate"

client = os.path.join(Test.TestDirectory, 'web.py')
for name, extra in (('gated', {'web': '"https to api.example.com"'}),
                    ('bare', {})):
  run = Test.AddTestRun(name)
  sock = driver.endpoint()
  run.Processes.Default.Command = (
    f'{islands.binary(Test.TestDirectory)} '
    f'{islands.flags(frames=0, cartridge="none", **extra)} '
    f'--listen {sock} > {name}.engine.out 2>&1 & '
    f'python3 {client} {sock} {name} || kill $$!; wait $$!')
  run.Processes.Default.ReturnCode = 0
  run.Processes.Default.Streams.stdout = islands.gold(
    Test.TestDirectory, config, f'web.{name}.gold')
