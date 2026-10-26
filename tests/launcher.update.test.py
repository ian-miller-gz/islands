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

Test.Summary = "The launcher follows a line, adds a link and updates it"

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

first = '0.1.0'
second = '0.1.1'
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
  f'echo "release: {first}" >> islands-box/manifest.yaml && '
  f'git -C islands-box init -q && git -C islands-box add -A && '
  f'git -C islands-box {walker} commit -qm box && '
  f'git -C islands-box branch stable && '
  f'git clone -q --bare islands-box islands-box.git && '
  f'chmod a-w home || exit 1')

settings = (
  f'"echo launcher" "target assets/launcher.gui" "click settings" '
  f'"read line" "click line" "click sline1" "read line" "click line" '
  f'"click sline0" "read line" "read linestate"')
store = (
  f'"target assets/launcher.gui" "click store" "hold unreached read state0" '
  f'"click linkadd" "click linkkeep" "read linknote" '
  f'"type $$(pwd)/islands-box.git" "click linkkeep" "read linkstate" '
  f'"click linkkeep" "hold box read name1" "hold {first} read release1" '
  f'"click linkadd" "type $$(pwd)/islands-box.git" "click linkkeep" '
  f'"read linknote" "click linkclose" "click linkkeep" "read name2" '
  f'"click install1" "hold \u2713|failed read state1" "read install1" '
  f'"click update1"')
bump = (
  f'git clone -q --branch stable islands-box.git bump && '
  f'sed -i "s/^release: .*/release: {second}/" bump/manifest.yaml && '
  f'git -C bump {walker} commit -qam bump && '
  f'git -C bump push -q origin stable && echo "stable holds {second}"')
update = (
  f'"target assets/launcher.gui" "click store" "click store" '
  f'"hold {second} read release1" "read state1" "click update1" '
  f'"hold \u2713|failed read state1" "read install1" "click update1"')
manifest = 'state/islands/cartridges/box/manifest.yaml'
placed = (
  f'grep -qx "release: {second}" {manifest} && '
  f'echo "box manifest holds {second}"')
library = (
  f'"target assets/launcher.gui" "click library" "click tree.0" '
  f'"read pname" "click pupdate"')

run = Test.AddTestRun('update')
sock = driver.endpoint()
paced = f'PACED_FRAME=1 python3 {client} {sock}'
run.Processes.Default.Command = (
  f'{sandbox}; '
  f'XDG_STATE_HOME=$$(pwd)/state ./home/Island '
  f'{islands.flags(frames=0, cartridge=bundle, width=1280, height=800)} '
  f'--web "https to 127.0.0.1" '
  f'--listen {sock} > launcher.update.engine.out 2>&1 & '
  f'{paced} {settings} || kill $$!; '
  f'{paced} {store} || kill $$!; {bump}; '
  f'{paced} {update} || kill $$!; {placed}; '
  f'{paced} {library} quit || kill $$!; '
  f'wait $$!; code=$$?; chmod u+w home; exit $$code')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 400
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'launcher.update.gold')
