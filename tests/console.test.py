import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)
compare = os.path.join(Test.TestDirectory, 'compare.py')
frame = os.path.join(Test.RunDirectory, 'console.ppm')
again = os.path.join(Test.RunDirectory, 'console.restart.ppm')

Test.Summary = "The glyph console matches the pixel golds and survives a restart"

run = Test.AddTestRun("capture")
run.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} '
  f'{islands.flags(overlay=1, window="debug=0", cartridge="none", capture=frame)} '
  f'--window console=1')
run.Processes.Default.ReturnCode = 0
run.Disk.File(frame, exists=True)

exact = Test.AddTestRun("match")
exact.Processes.Default.Command = (
  f'python3 {compare} '
  f'{islands.gold(Test.TestDirectory, config, "console.ppm")} {frame} 0')
exact.Processes.Default.ReturnCode = 0

other = 'sdl' if config == 'vulkan' else 'vulkan'
sibling = islands.gold(Test.TestDirectory, other, 'console.ppm')
if os.path.exists(sibling):
  cross = Test.AddTestRun("cross")
  cross.Processes.Default.Command = (
    f'python3 {compare} {sibling} {frame} {islands.TOLERANCE}')
  cross.Processes.Default.ReturnCode = 0

cycled = islands.flags(
  overlay=1, window='debug=0', cartridge='none', capture=again, restarts=1)
cycled += ' --window console=1'
restart = Test.AddTestRun("restart")
restart.Processes.Default.Command = (
  f'{islands.binary(Test.TestDirectory)} {cycled}')
restart.Processes.Default.ReturnCode = 0
restart.Disk.File(again, exists=True)

hygiene = Test.AddTestRun("hygiene")
hygiene.Processes.Default.Command = (
  f'python3 {compare} '
  f'{islands.gold(Test.TestDirectory, config, "console.ppm")} {again} 0')
hygiene.Processes.Default.ReturnCode = 0
