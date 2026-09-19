import os
import sys
sys.path.insert(0, Test.TestDirectory)
import islands

config = islands.backend(Test.TestDirectory)
islands.dynamic(Test, Condition)

Test.Summary = "The island host initializes, renders a bounded run, and shuts down"

binary = islands.binary(Test.TestDirectory)
host = os.path.basename(binary)
root = islands.root(Test.TestDirectory)

run = Test.AddTestRun("lifecycle")
run.ContinueOnFail = True
run.Processes.Default.Command = (
  f'{binary} {islands.flags()}')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'lifecycle.gold')

run = Test.AddTestRun("state")
run.ContinueOnFail = True
run.Processes.Default.Command = (
  f'mkdir -p home state && '
  f'(ln {binary} home/{host} 2>/dev/null || cp {binary} home/{host}) && '
  f'ln -s {root}/assets home/assets && chmod a-w home && '
  f'XDG_STATE_HOME=$$(pwd)/state ./home/{host} '
  f'{islands.flags(cartridge="none")} > lifecycle.state.out 2>&1; '
  f'code=$$?; chmod u+w home; echo "host $$code"; '
  f'grep -cF "/state/islands/logs/islands.log" lifecycle.state.out; '
  f'ls state/islands/logs')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 120
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'lifecycle.state.gold')

run = Test.AddTestRun("floor")
run.Processes.Default.Command = (
  f'mkdir -p temp && chmod a-w home && '
  f'env -u HOME -u XDG_STATE_HOME TMPDIR=$$(pwd)/temp ./home/{host} '
  f'{islands.flags(cartridge="none")} > lifecycle.floor.out 2>&1; '
  f'code=$$?; chmod u+w home; echo "host $$code"; '
  f'grep -cF "/temp/islands/logs/islands.log" lifecycle.floor.out; '
  f'ls temp/islands/logs')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 120
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'lifecycle.floor.gold')

run = Test.AddTestRun("version")
run.Processes.Default.Command = (
  f'env -u DISPLAY -u WAYLAND_DISPLAY {binary} --version; echo "island $$?"; '
  f'env -u DISPLAY -u WAYLAND_DISPLAY {os.path.join(os.path.dirname(binary), "Reef")} '
  f'--version; echo "reef $$?"')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, config, 'lifecycle.version.gold')
