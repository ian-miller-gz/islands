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

Test.Summary = "The launcher follows a line, adds a link, updates it in sight and surfaces a failed build and a newer engine"

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
third = '0.1.2'
fourth = '0.1.3'
fifth = '0.1.4'
stated = '9.9.9'
mark = '\u2713'
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
  f'"read line" "click line" "click releaseentry1" "read line" '
  f'"click line" "click releaseentry0" "read line" "read linestate"')
store = (
  f'"target assets/launcher.gui" "click store" "hold unreached read state0" '
  f'"click linkadd" "click sourceadd" "read sourcestate" '
  f'"type $$(pwd)/islands-box.git" "click sourceadd" "read sourcestate" '
  f'"click sourceclose" "click sourceadd" "hold box read name1" '
  f'"hold {first} read release1" '
  f'"click linkadd" "type $$(pwd)/islands-box.git" "click sourceadd" '
  f'"read sourcestate" "click sourceclose" "click sourceadd" "read name2" '
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


def placed(release, word='holds'):
  return (
    f'grep -qx "release: {release}" {manifest} && '
    f'echo "box manifest {word} {release}"')


def bumped(release, change=''):
  return (
    f'{change}sed -i "s/^release: .*/release: {release}/" bump/manifest.yaml && '
    f'git -C bump {walker} commit -qam {release} && '
    f'git -C bump push -q origin stable && echo "stable holds {release}"')


gauge = (
  f'"target assets/launcher.gui" "click store" "click store" '
  f'"hold {third} read release1" "click library" "click tree.0" '
  f'"read pname" "read pstate" "click pupdate" '
  f'"hold building read pstate" "read pprogress" "click store" '
  f'"hold {mark}|failed read state1" "click library" "click tree.0" '
  f'"read pstate"')
broken = (
  f'"target assets/launcher.gui" "click store" "hold {fourth} read release1" '
  f'"click update1" "hold failed read noticeline" "read state1" '
  f'"click noticeclose" "click library" "click tree.0" "read pstate"')
engine = (
  f'"target assets/launcher.gui" "click store" "hold {fifth} read release1" '
  f'"click update1" "hold checking read noticeline" "click noticeclose" '
  f'"click library" "click tree.0" "read pstate"')

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
  f'{paced} {update} || kill $$!; {placed(second)}; {bumped(third)}; '
  f'{paced} {gauge} || kill $$!; {placed(third)}; '
  f'{bumped(fourth, "echo broken >> bump/cartridge.cpp && ")}; '
  f'{paced} {broken} || kill $$!; '
  f'{bumped(fifth, f"echo islands: {stated} >> bump/manifest.yaml && ")}; '
  f'{paced} {engine} quit || kill $$!; {placed(third, "still holds")}; '
  f'wait $$!; code=$$?; chmod u+w home; exit $$code')
run.Processes.Default.ReturnCode = 0
run.Processes.Default.TimeOut = 600
run.Processes.Default.Streams.stdout = islands.gold(
  Test.TestDirectory, islands.skin(Test.TestDirectory, config),
  'launcher.update.gold')
