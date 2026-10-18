import os
import shutil
import sys
sys.path.insert(0, Test.TestDirectory)
import islands
import driver

config = islands.backend(Test.TestDirectory)
islands.listening(Test, Condition)
islands.dynamic(Test, Condition)
islands.native(Test, Condition)
islands.debug(Test, Condition)

Test.Summary = "The launcher adds a local line on a row, updates it by tip and keeps the launcher's own line apart"

binary = islands.binary(Test.TestDirectory)
reef = os.path.join(os.path.dirname(binary), 'Reef')
client = os.path.join(Test.TestDirectory, 'paced.py')
bundle = 'cartridges/.core/launcher'
root = islands.root(Test.TestDirectory)
box = os.path.join(root, 'cartridges', '.test', 'box')
Test.SkipIf(Condition(
  lambda: not os.path.exists(os.path.join(root, bundle, 'posix',
                                          'liblauncher.so')),
  'the launcher is not built (python3 -m make)'))
Test.SkipIf(Condition(
  lambda: shutil.which('git') is None,
  'git is not on this desk (the link fixture is a repository)'))

release = '0.1.0'
uat = '0.2-uat'
mark = '✓'
walker = '-c user.email=walk@islands -c user.name=walk'
sandbox = (
  f'mkdir -p home/cartridges state/islands/configs && '
  f'(ln {binary} home/Island || cp {binary} home/Island) && '
  f'(ln {reef} home/Reef || cp {reef} home/Reef) && '
  f'ln -s {root}/assets home/assets && '
  f'ln -s {root}/configs home/configs && '
  f'ln -s {root}/cartridges/.core home/cartridges/.core && '
  f'ln -s {root}/cartridges/build.sh home/cartridges/build.sh && '
  f'ln -s {root}/src home/src && ln -s {root}/libs home/libs && '
  f'ln -s {root}/submodules home/submodules && mkdir -p home/build && '
  f'ln -s {root}/build/include home/build/include && '
  f'ln -s {root}/build/outputs home/build/outputs && '
  f'cp -r {box} islands-box && rm -rf islands-box/posix && '
  f'echo "release: {release}" >> islands-box/manifest.yaml && '
  f'git -C islands-box init -q -b stable && git -C islands-box add -A && '
  f'git -C islands-box {walker} commit -qm box && '
  f'git -C islands-box branch latest && '
  f'git -C islands-box checkout -q -b {uat} && '
  f'git -C islands-box {walker} commit -q --allow-empty -m uat && '
  f'git -C islands-box checkout -q stable && '
  f'git clone -q --bare islands-box islands-box.git && '
  f'chmod a-w home || exit 1')

bare = '$$(pwd)/islands-box.git'
lines = 'state/islands/configs/lines.yaml'
sources = 'state/islands/configs/sources.yaml'
followed = 'state/islands/configs/release.yaml'
tip = f'$$(git -C islands-box.git rev-parse {uat})'
store = (
  f'"echo launcher" "target assets/launcher.gui" "click store" '
  f'"hold unreached read state0" "click linkadd" "type {bare}" '
  f'"click linkkeep" "hold box read name1" "read sline" "read rline1" '
  f'"click install1" "hold {mark}|failed read state1" "read install1" '
  f'"click update1"')
card = (
  f'"target assets/launcher.gui" "click radd1" "read sourcename" '
  f'"read source" "read side" "click find" '
  f'"hold found|none|unreached read sourcestate" "read found.0" '
  f'"click found.0" "click sourceadd" "read sourcestate" '
  f'"read sourcelist.0" "click sourceclose"')
kept = (
  f'grep -qx "  - leaf: box" {sources} && '
  f'grep -qx "    repository: {bare}" {sources} && '
  f'grep -qx "    branch: {uat}" {sources} && '
  f'grep -qx "    local: true" {sources} && '
  f'echo "sources.yaml holds {uat} for box"')
pick = (
  f'"target assets/launcher.gui" "click rline1" "read rentry2" '
  f'"click rentry2" "read rline1" "read sline"')
written = (
  f'grep -qx "box: {uat}" {lines} && test $$(wc -l < {lines}) = 1 && '
  f'echo "lines.yaml holds box: {uat}"')
update = (
  f'"target assets/launcher.gui" "hold box read name1" "read release1" '
  f'"click update1" "hold {mark}|failed read state1" "read install1" '
  f'"click update1"')
recorded = (
  f'grep -qx "box: {{line: {uat}, tip: {tip}}}" {lines} && '
  f'echo "lines.yaml records the {uat} tip"')
push = (
  f'git clone -q --branch {uat} islands-box.git push && '
  f'git -C push {walker} commit -q --allow-empty -m moved && '
  f'git -C push push -q origin {uat} && echo "{uat} moves"')
moved = (
  f'"target assets/launcher.gui" "click store" "click store" '
  f'"hold box read name1" "click update1" '
  f'"hold {mark}|failed read state1" "click update1"')
refused = (
  f'"target assets/launcher.gui" "click radd1" "read sourcestate" '
  f'"click source" "key end" "key home shift" "type $$(pwd)/nowhere" '
  f'"click find" "hold unreached|found|none read sourcestate" '
  f'"click source" "key end" "key home shift" "type {bare}" "key enter" '
  f'"hold none|found|unreached read sourcestate" "click found.0"')
removal = (
  f'"target assets/launcher.gui" "click sourcelist.0" "click sourcedrop" '
  f'"read sourcestate" "click sourcelist.0" "click sourceclose" '
  f'"hold stable read rline1"')
dropped = (
  f'! grep -q "branch: {uat}" {sources} && cp {lines} lines.before && '
  f'echo "sources.yaml drops {uat}"')
top = (
  f'"target assets/launcher.gui" "click sline" "read sline1" '
  f'"click sline1" "read sline" "hold latest read rline1"')
apart = (
  f'grep -qx "release: latest" {followed} && cmp -s {lines} lines.before && '
  f'echo "release.yaml follows latest, lines.yaml stands"')
settings = (
  f'"target assets/launcher.gui" "click settings" "read line" "click line" '
  f'"read sline0" "click sline0" "read line" "click sline1"')

run = Test.AddTestRun('lines')
sock = driver.endpoint()
paced = f'PACED_FRAME=1 python3 {client} {sock}'
run.Processes.Default.Command = (
  f'{sandbox}; '
  f'XDG_STATE_HOME=$$(pwd)/state ./home/Island '
  f'{islands.flags(frames=0, cartridge=bundle, width=1280, height=800)} '
  f'--web "https to 127.0.0.1" '
  f'--listen {sock} > launcher.lines.engine.out 2>&1 & '
  f'{paced} {store} || kill $$!; '
  f'{paced} {card} || kill $$!; {kept}; '
  f'{paced} {pick} || kill $$!; {written}; '
  f'{paced} {update} || kill $$!; {recorded}; {push}; '
  f'{paced} {moved} || kill $$!; {recorded}; '
  f'{paced} {refused} || kill $$!; '
  f'{paced} {removal} || kill $$!; {dropped}; '
  f'{paced} {top} || kill $$!; {apart}; '
  f'{paced} {settings} quit || kill $$!; '
  f'wait $$!; code=$$?; chmod u+w home; exit $$code')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 600
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'launcher.lines.gold')
