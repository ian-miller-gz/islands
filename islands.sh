#!/bin/sh
set -eu

home="$(cd "$(dirname "$0")" && pwd)"

launcher="$home/cartridges/.core/launcher"
if [ ! -d "$launcher" ]; then
  line="$(sed -n 's/^release:[[:space:]]*//p' "$home/configs/release.yaml" 2>/dev/null | head -n1)"
  [ -n "$line" ] || line=stable
  echo "Cloning the launcher ($line) into $launcher..." >&2
  git clone --quiet --branch "$line" --depth 1 \
    https://github.com/ian-miller-gz/islands-launcher "$launcher" || exit 1
fi

bundle=
previous=
for argument in "$@"; do
  [ "$previous" = "--cartridge" ] && bundle="$argument"
  previous="$argument"
done
[ -n "$bundle" ] || bundle="cartridges/.core/launcher"

manifest="$bundle/manifest.yaml"
[ -f "$manifest" ] || manifest="$home/$bundle/manifest.yaml"

needed=
for host in Island Reef; do
  [ -x "$home/build/$host" ] || needed=1
done
if [ -f "$manifest" ]; then
  entry="$(sed -n 's/^entry:[[:space:]]*//p' "$manifest")"
  directory="$(dirname "$manifest")"
  if [ -n "$entry" ]; then
    [ -f "$directory/posix/$entry" ] || [ -f "$directory/$entry" ] || needed=1
  fi
fi
if [ -n "$needed" ]; then
  python="$home/.venv/bin/python3"
  [ -x "$python" ] || python=python3
  log="$home/logs/build/islands.sh.log"
  mkdir -p "$home/logs/build"
  echo "Building missing artifacts (transcript: $log)..." >&2
  if ! (cd "$home" && "$python" -m make) >"$log" 2>&1; then
    tail -40 "$log" >&2
    echo "Build failed — full transcript: $log" >&2
    exit 1
  fi
fi
exec "$home/build/Island" "$@"
