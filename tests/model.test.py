import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
compare = os.path.join(Test.TestDirectory, 'compare.py')
frame = os.path.join(Test.RunDirectory, 'model.ppm')

Test.Summary = "The imported model matches the pixel golds and refuses loudly"

run = Test.AddTestRun("capture")
run.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} {islands.flags(capture=frame, cartridge="cartridges/.test/box")}')
run.Processes.Default.ReturnCode = 0
run.Disk.File(frame, exists=True)

exact = Test.AddTestRun("match")
exact.Processes.Default.Command = (
  f'python3 {compare} '
  f'{islands.gold(Test.TestDirectory, config, "model.ppm")} {frame} 0')
exact.Processes.Default.ReturnCode = 0

other = 'sdl' if config == 'vulkan' else 'vulkan'
sibling = islands.gold(Test.TestDirectory, other, 'model.ppm')
if os.path.exists(sibling):
  cross = Test.AddTestRun("cross")
  cross.Processes.Default.Command = (
    f'python3 {compare} {sibling} {frame} {islands.TOLERANCE}')
  cross.Processes.Default.ReturnCode = 0

lines = Test.AddTestRun("lines")
lines.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} '
  f'{islands.flags(cartridge="tests/fixtures/lines")}')
lines.Processes.Default.ReturnCode = 0
lines.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'model.lines.gold')
