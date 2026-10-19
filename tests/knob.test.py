import os
import sys

sys.path.insert(0, Test.TestDirectory)
import islands

Test.Summary = "The retained GUI knob: the round dial's gestures (GUI::)"

config = islands.backend(Test.TestDirectory)
fixture = os.path.join(
  islands.root(Test.TestDirectory), 'build', 'outputs', 'gui.out')

run = Test.AddTestRun('knob')
run.Processes.Default.Command = (
  f'{fixture} knob > knob.out; status=$$?; cat knob.out; exit $$status')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config), 'knob.gold')
islands.counted(Test, r'^gathered notch$', 1, 'knob.out')
