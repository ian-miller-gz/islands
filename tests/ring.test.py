import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The keys' reach: buttons in the ring, Enter a click, the rim"

Test.SkipIf(Condition(
  lambda: islands.token(
    Test.TestDirectory, 'SR_GUI_BACKEND', 'SR_NATIVE') != 'SR_NATIVE',
  'the RmlUi backend draws no rim (SR_GUI_BACKEND: SR_RMLUI)'))

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('ring')
run.Processes.Default.Command = f'{fixture} ring'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'ring.gold')
