import os
import signal
import subprocess
import sys

import driver

FIXTURES = 'tests/fixtures/'
ROSTER = 'cartridges/.test/roster'


def engine(binary, flags, home, bundle, **extra):
  overrides = [word for key, value in extra.items()
               for word in (f'--{key}', str(value))]
  return driver.launch(
    [binary] + flags + ['--cartridge', FIXTURES + bundle] + overrides, home,
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)


def lenient(binary, flags):
  home = driver.home()
  first = engine(binary, flags, home, 'daemons')
  watcher = driver.Reader(first.stdout)
  try:
    driver.until(lambda: watcher.has('started.'), 'the daemon coming up')
    print('ensured: the daemon came up')
    second = engine(binary, flags, home, 'daemons', frames=30)
    output, _ = second.communicate(timeout=driver.DEADLINE)
    print('joined:', 'the second engine reused it'
          if 'joined.' in output and 'started.' not in output
          else 'unexpected\n' + output)
    running = driver.hosts(home)
    print('singleton:', 'one host process' if len(running) == 1
          else f'{len(running)} host processes')
    for pid in running:
      os.kill(pid, signal.SIGKILL)
    driver.until(lambda: watcher.has('died.'), 'the death notice')
    driver.tell(first, 'quit')
    status = first.wait(timeout=driver.DEADLINE)
    print('died:', 'the run continued' if status == 0
          else f'unexpected exit {status}')
  finally:
    driver.down(first)
    driver.reap(home)


def vital(binary, flags):
  home = driver.home()
  refused = engine(binary, flags, home, 'vital', frames=30)
  output, _ = refused.communicate(timeout=driver.DEADLINE)
  print('vital:', 'refused while unreachable'
        if refused.returncode == 1 and 'unavailable.' in output
        else f'unexpected exit {refused.returncode}\n' + output)
  host = driver.host(binary, ROSTER, home)
  try:
    driver.until(lambda: driver.up(
      driver.rendezvous(home, 'roster')), 'the daemon socket')
    run = engine(binary, flags, home, 'vital')
    watcher = driver.Reader(run.stdout)
    driver.until(lambda: watcher.has('joined.'), 'the registration')
    host.send_signal(signal.SIGKILL)
    status = run.wait(timeout=driver.DEADLINE)
    print('vital:', 'the death ended the run'
          if status != 0 and watcher.has('died.')
          else f'unexpected exit {status}')
  finally:
    if host.poll() is None:
      host.kill()
    host.wait()


def policy(binary, flags):
  run = engine(binary, flags, driver.home(), 'policy', frames=30)
  output, _ = run.communicate(timeout=driver.DEADLINE)
  print('policy:', 'the typo was refused'
        if run.returncode == 1 and 'Unknown requirement policy' in output
        else f'unexpected exit {run.returncode}\n' + output)


def main():
  binary, flags = sys.argv[1], sys.argv[2:]
  lenient(binary, flags)
  vital(binary, flags)
  policy(binary, flags)
  return 0


if __name__ == '__main__':
  sys.exit(main())
