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

Test.Summary = "The launcher opens bare, Library stands the page, About shows the version"

binary = islands.binary(Test.TestDirectory)
client = os.path.join(Test.TestDirectory, 'paced.py')
bundle = 'cartridges/.core/launcher'
root = islands.root(Test.TestDirectory)
Test.SkipIf(Condition(
  lambda: not os.path.exists(os.path.join(root, bundle, 'posix',
                                          'liblauncher.so')),
  'the launcher is not built (python3 -m make)'))

run = Test.AddTestRun('about')
sock = driver.endpoint()
run.Processes.Default.Command = (
  f'{binary} {islands.flags(frames=0, cartridge=bundle)} '
  f'--listen {sock} > launcher.about.engine.out 2>&1 & '
  f'PACED_FRAME=0.5 python3 {client} {sock} "echo launcher" '
  f'"target assets/launcher.gui" "read pname" "read general" '
  f'"click library" "read general" "click gear" "read setupname" '
  f'"read box0" "click setupclose" "read setupname" "read about" '
  f'"read noticeline" '
  f'"click about" "read noticeline" "read noticeclose" "click noticeclose" '
  f'"read noticeline" quit || kill $$!; wait $$!')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 120
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'launcher.about.gold')
