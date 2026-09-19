import subprocess
import sys

import driver


def subscribe(path):
  link = driver.connect(path)
  link.settimeout(driver.DEADLINE)
  link.sendall(b'subscribe\tframes\n')
  return link


def event(link):
  data = b''
  while b'\n' not in data:
    data += link.recv(4096)
  return data.decode().split('\n', 1)[0].split('\t')


def prove(fields):
  topic, frame, step = fields
  print('topic:', 'frames' if topic == 'frames' else f'unexpected {topic}')
  print('event:', 'well-formed'
        if int(frame) >= 1 and float(step) >= 0.0 else f'unexpected {fields}')


def answers(sock):
  link = driver.connect(sock)
  link.sendall(b'metrics\nquit\n')
  return b'Rendered' in driver.drain(link)


def main():
  binary, flags = sys.argv[1], sys.argv[2:]
  home = driver.home()
  well = driver.rendezvous(home, 'messages')
  sock = driver.endpoint()
  host = driver.host(binary, 'cartridges/.test/messages', home)
  engine = None
  try:
    driver.until(lambda: driver.up(well), 'the daemon socket')
    follower = subscribe(well)
    engine = driver.launch(
      [binary] + flags + ['--listen', sock], home,
      stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
    prove(event(follower))
    print('published: frames flowing')
    host.kill()
    host.wait(timeout=driver.DEADLINE)
    print('broker killed:',
          'engine answers' if answers(sock) else 'engine unresponsive')
    return engine.wait(timeout=driver.DEADLINE)
  finally:
    driver.down(engine)
    if host.poll() is None:
      host.terminate()
      host.wait(timeout=driver.DEADLINE)


if __name__ == '__main__':
  sys.exit(main())
