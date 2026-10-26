import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The stroke kind: one run per shape word, read back as its pieces"

Test.SkipIf(Condition(
  lambda: islands.token(
    Test.TestDirectory, 'SR_GUI_BACKEND', 'SR_NATIVE') != 'SR_NATIVE',
  'the RmlUi backend draws no strokes (SR_GUI_BACKEND: SR_RMLUI)'))

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('strokes')
run.Processes.Default.Command = f'{fixture} strokes'
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'strokes.gold')
