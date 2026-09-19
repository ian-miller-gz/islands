
import subprocess
import sys

import driver

TAB = '\t'


def escape(token):
  return token.replace('\\', '\\\\').replace('\t', '\\t').replace('\n', '\\n')


def unescape(token):
  out, at = [], 0
  while at < len(token):
    if token[at] == '\\' and at + 1 < len(token):
      nxt = token[at + 1]
      out.append('\t' if nxt == 't' else '\n' if nxt == 'n' else nxt)
      at += 2
    else:
      out.append(token[at])
      at += 1
  return ''.join(out)


def subscribe(topic):
  return ('S' + TAB + escape(topic) + '\n').encode()


def publish(topic, fields):
  parts = ['P', escape(topic)]
  for name, tag, value in fields:
    parts += [escape(name), tag, escape(value)]
  return (TAB.join(parts) + '\n').encode()


def line(sock):
  data = b''
  while b'\n' not in data:
    data += sock.recv(4096)
  return data.split(b'\n', 1)[0].decode()


def decode(text):
  tokens = text.split(TAB)
  if len(tokens) < 2:
    return None, None, {}
  fields, at = {}, 2
  while at + 2 < len(tokens):
    name, tag, raw = (
      unescape(tokens[at]),
      tokens[at + 1],
      unescape(tokens[at + 2]),
    )
    fields[name] = (
      int(raw)
      if tag in ('i', 'w')
      else float(raw)
      if tag == 'f'
      else raw == '1'
      if tag == 'b'
      else raw
    )
    at += 3
  return tokens[0], unescape(tokens[1]), fields


def forwarded(peer):
  verb, topic, fields = decode(line(peer))
  good = (
    verb == 'P'
    and topic == 'up'
    and fields.get('n') == 42
    and fields.get('who') == 'client'
  )
  print('forwarded:', 'well-formed' if good else f'unexpected {topic} {fields}')
  return good


def main():
  fixture = sys.argv[1]
  home = driver.home()
  well = driver.rendezvous(home, 'topics-wire')
  broker = driver.launch(
    [fixture, 'broker'],
    home,
    stdout=subprocess.DEVNULL,
    stderr=subprocess.STDOUT,
  )
  client = None
  try:
    driver.until(lambda: driver.up(well), 'the broker socket')
    peer = driver.connect(well)
    peer.settimeout(driver.DEADLINE)
    peer.sendall(subscribe('up'))
    client = driver.launch(
      [fixture, 'client'],
      home,
      stdout=subprocess.DEVNULL,
      stderr=subprocess.STDOUT,
    )
    outbound = forwarded(peer)
    peer.sendall(publish('down', [('n', 'w', '99')]))
    received = client.wait(timeout=driver.DEADLINE) == 0
    print('injected:', 'delivered' if received else 'not received')
    return 0 if outbound and received else 1
  finally:
    driver.down(client, broker)


if __name__ == '__main__':
  sys.exit(main())
