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

Test.Summary = "The launcher's one Add keeps a local row on a found line, then a source alone"

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
  'git is not on this desk (the scratch cartridge is a repository)'))

release = '0.1.0'
marked = '0.2.0'
first = '0.2-uat'
second = '0.3-uat'
walker = '-c user.email=walk@islands -c user.name=walk'
scratch = 'islands-scratch'
sandbox = (
  f'mkdir -p home/cartridges state/islands/configs {scratch} && '
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
  f'printf "name: scratch\\nentry: libscratch.so\\n" '
  f'> {scratch}/manifest.yaml && '
  f'grep "^version:" {box}/manifest.yaml >> {scratch}/manifest.yaml && '
  f'echo "release: {release}" >> {scratch}/manifest.yaml && '
  f'echo "extern \\"C\\" void scratch() {{}}" > {scratch}/cartridge.cpp && '
  f'git -C {scratch} init -q -b stable && git -C {scratch} add -A && '
  f'git -C {scratch} {walker} commit -qm scratch && '
  f'git -C {scratch} branch latest && git -C {scratch} branch {second} && '
  f'git -C {scratch} checkout -q -b {first} && '
  f'sed -i "s/^release: .*/release: {marked}/" {scratch}/manifest.yaml && '
  f'git -C {scratch} {walker} commit -qam uat && '
  f'git -C {scratch} checkout -q stable && '
  f'chmod a-w home || exit 1')

path = f'$$(pwd)/{scratch}'
links = 'state/islands/configs/links.yaml'
sources = 'state/islands/configs/sources.yaml'
lines = 'state/islands/configs/lines.yaml'
add = (
  f'"echo launcher" "target assets/launcher.gui" "click store" '
  f'"hold unreached read state0" "click linkadd" "read sourcename" '
  f'"read source" "read side" "type {path}" "read side" "click find" '
  f'"hold found|none|unreached read sourcestate" "read side" '
  f'"read found.0" "read found.1" "click found.0" "click sourceadd" '
  f'"read sourcestate" "read sourcelist.0" "click sourceclose" '
  f'"hold scratch read name1" "hold {marked} read release1" '
  f'"read rline1" "read name2"')
kept = (
  f'grep -qx "  - {path}" {links} && '
  f'grep -qx "    branch: {first}" {sources} && '
  f'grep -qx "scratch: {first}" {lines} && '
  f'echo "links.yaml holds the scratch row on {first}"')
again = (
  f'"target assets/launcher.gui" "click linkadd" "type {path}" '
  f'"click find" "hold found|none|unreached read sourcestate" '
  f'"read found.0" "click found.0" "click sourceadd" "read sourcestate" '
  f'"read sourcelist.1" "click sourceclose" "hold {second} read rline1" '
  f'"read name2"')
alone = (
  f'test $$(grep -c "^  - " {links}) = 1 && '
  f'grep -qx "    branch: {first}" {sources} && '
  f'grep -qx "    branch: {second}" {sources} && '
  f'grep -qx "scratch: {second}" {lines} && '
  f'echo "links.yaml stands, sources.yaml holds {first} and {second}"')

run = Test.AddTestRun('add')
sock = driver.endpoint()
paced = f'PACED_FRAME=1 python3 {client} {sock}'
run.Processes.Default.Command = (
  f'{sandbox}; '
  f'XDG_STATE_HOME=$$(pwd)/state ./home/Island '
  f'{islands.flags(frames=0, cartridge=bundle, width=1280, height=800)} '
  f'--web "https to 127.0.0.1" '
  f'--listen {sock} > launcher.add.engine.out 2>&1 & '
  f'{paced} {add} || kill $$!; {kept}; '
  f'{paced} {again} quit || kill $$!; {alone}; '
  f'wait $$!; code=$$?; chmod u+w home; exit $$code')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 300
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'launcher.add.gold')
