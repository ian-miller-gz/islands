import os
import signal
import subprocess
import sys
import time

import driver

ROOT = os.path.abspath(
  os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir))


def line(link):
  data = b''
  while b'\n' not in data:
    chunk = link.recv(4096)
    if not chunk:
      return None
    data += chunk
  return data.split(b'\n', 1)[0].decode()


def deliver(publisher, subscriber, topic, payload):
  limit = time.time() + driver.DEADLINE
  subscriber.settimeout(0.2)
  while time.time() < limit:
    publisher.sendall(f'publish\t{topic}\t{payload}\n'.encode())
    try:
      return line(subscriber)
    except TimeoutError:
      continue
  raise TimeoutError('the delivery never happened')


def main():
  binary = sys.argv[1]
  home = driver.home()
  broker = driver.rendezvous(home, 'messages')
  jetty = driver.launch(
    [binary, '--cartridge', 'cartridges/.test/messages'], home, cwd=ROOT,
    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
  reader = driver.Reader(jetty.stdout)
  try:
    driver.until(lambda: driver.up(broker), 'the broker address')
    print('broker: up')
    driver.until(lambda: reader.has('jetty console: ahoy'), 'the echo answer')
    print('console: echo answers')
    subscriber = driver.connect(broker)
    subscriber.sendall(b'subscribe\ttelemetry\n')
    publisher = driver.connect(broker)
    landed = deliver(publisher, subscriber, 'telemetry', 'ping')
    print('delivered:', landed.replace('\t', ' '))
    publisher.close()
    subscriber.close()
    jetty.send_signal(signal.SIGTERM)
    code = jetty.wait(timeout=driver.DEADLINE)
    driver.until(
      lambda: reader.has('jetty teardown: complete'), 'the teardown line')
    print(f'teardown: complete, exit {code}')
    return code
  finally:
    driver.down(jetty)


if __name__ == '__main__':
  sys.exit(main())
