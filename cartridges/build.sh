#!/usr/bin/env bash
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(dirname "$here")"
codegen=""
if [ "${1:-}" = "--optimized" ]; then codegen="-O2 -DNDEBUG"; shift; fi
usage="usage: cartridges/build.sh [--optimized] <bundle-dir> [windows]"
bundle="${1:?$usage}"
platform="${2:-}"
case "$platform" in "" | windows) ;; *) echo "$usage" >&2; exit 2 ;; esac
cd "$root"

bundle="${bundle%/}"
manifest="$bundle/manifest.yaml"
[ -f "$manifest" ] || { echo "No manifest at $manifest" >&2; exit 1; }

toolchain="$(sed -n '/^toolchains:/,$p' configs/make.yaml |
                 sed -n "/^  ${platform:-none}:/,/^  [a-z]/p")"
scalar() {
  printf '%s\n' "$toolchain" | sed -n "s/^$2$1:[[:space:]]*//p" | head -n1
}
flow() {
  printf '%s\n' "$toolchain" |
    sed -n "s/^    $1:[[:space:]]*\[\(.*\)\][[:space:]]*\$/\1/p" |
    sed 's/, /\n/g' | tr -d '"'
}
fold="${platform:-posix}"
suffix=".so"
cxx="g++"
codeflags="-fPIC -fno-gnu-unique"
linkflags="$codeflags"
implib=""
imports=""
seat=""
engine=""
profile="$(sed -n 's/^[[:space:]]*SR_PROFILE:[[:space:]]*\(SR_[A-Z]*\).*/\1/p' \
             configs/make.yaml | head -n1)"
zone="libs/$(printf '%s' "${profile#SR_}" | tr '[:upper:]' '[:lower:]')"
if [ -n "$platform" ]; then
  cxx="$(scalar compiler '    ')"
  engine="$(scalar engine '    ')"
  suffix="$(scalar library '      ')"
  seat="$(scalar directory '      ')/outputs"
  zone="$(scalar "$profile" '      ')"
  codeflags="$(flow compile_flags | tr '\n' ' ')"
  linkflags="$(flow link_flags | tr '\n' ' ')"
  implib="$seat/lib$engine$suffix.a"
  for name in $(flow imports); do imports="$imports -l$name"; done
  command -v "$cxx" >/dev/null ||
    { echo "No $cxx on PATH: the $platform toolchain is not installed." >&2
      exit 1; }
  [ -f "$implib" ] ||
    { echo "No engine import library at $implib — build the engine's shared" \
           "cell first: python3 -m make --configuration $platform-shared" >&2
      exit 1; }
  token="SR_$(printf '%s' "$platform" | tr '[:lower:]' '[:upper:]')"
  grep -q "SR_PLATFORM $token" build/include/generated/platform.hpp ||
    { echo "build/include/generated names another platform — re-run" \
           "python3 -m make --configuration $platform-shared" >&2
      exit 1; }
fi

name="$(sed -n 's/^entry:[[:space:]]*//p' "$manifest" | head -n1 | awk '{print $1}')"
if [ -n "$platform" ]; then
  delta="$(sed -n '/^platforms:/,$p' "$manifest" |
           sed -n "/^  $platform:/,/^  [a-z]/p" |
           sed -n 's/^    entry:[[:space:]]*//p' | head -n1 | awk '{print $1}')"
  [ -z "$delta" ] || name="$delta"
fi
[ -n "$name" ] || name="lib$(basename "$bundle")$suffix"

mapfile -t sources < <(find "$bundle" -name '*.cpp' | sort)
[ "${#sources[@]}" -gt 0 ] || { echo "No .cpp files under $bundle" >&2; exit 1; }

isys=""
for d in $(find src -name '*.hpp' -exec dirname {} \; | sort -u) \
         src src/core cartridges/.core cartridges/.test cartridges; do
  isys="$isys -isystem $d"
done
iinc="-I build/include"
for d in $(sed -n '/^includes:/,/^[a-z]/s/^[[:space:]]*-[[:space:]]*//p' \
             configs/make.yaml); do
  iinc="$iinc -I $d"
done

archives=""
for a in $(sed -n '/^library_archives:/,/^[a-z]/s/^[[:space:]]*-[[:space:]]*//p' \
             configs/make.yaml); do
  a="${a//<prebuilt>/$zone}"
  [ -z "$seat" ] || a="${a/#build\/outputs/$seat}"
  if [ -f "$a" ]; then archives="$archives $a"; fi
done

key="$bundle"
key="${key#"$root"/}"
key="${key#./}"
key="${key#cartridges/}"
objects="build/bundles/$key"
[ -z "$platform" ] || objects="$objects.$platform"
[ -z "$codegen" ] || objects="$objects.optimized"

stale() {
  local header
  [ -f "$1" ] || return 0
  [ ! "$1" -ot "$2" ] || return 0
  [ -f "${1%.o}.d" ] || return 0
  for header in $(sed -e 's/^[^:]*://' -e 's/\\$//' "${1%.o}.d"); do
    [ -e "$header" ] || return 0
    [ ! "$1" -ot "$header" ] || return 0
  done
  return 1
}

export ISLANDS_COMPILER="$cxx"
export ISLANDS_CXX="-std=c++23 $codegen $codeflags $isys $iinc"

work="$(mktemp)"
trap 'rm -f "$work"' EXIT
targets=()
for src in "${sources[@]}"; do
  obj="$objects/${src#"$bundle"/}.o"
  targets+=("$obj")
  if stale "$obj" "$src"; then
    mkdir -p "$(dirname "$obj")"
    printf '%s\n%s\n' "$src" "$obj" >>"$work"
  fi
done

due=$(( $(wc -l <"$work") / 2 ))
if [ "$due" -gt 0 ]; then
  echo "Compiling $due of ${#sources[@]} source file(s) on $(nproc) cores..."
  xargs -d '\n' -P "$(nproc)" -n 2 sh -c \
    '$ISLANDS_COMPILER $ISLANDS_CXX -MMD -MF "${2%.o}.d" -c "$1" -o "$2"' sh <"$work"
else
  echo "All ${#sources[@]} object(s) current."
fi

out="$bundle/$fold/$name"
mkdir -p "$(dirname "$out")"
echo "Linking $out..."
"$cxx" -std=c++23 $codegen $linkflags -shared "${targets[@]}" $archives \
  $implib $imports -o "$out"
if [ -n "$platform" ]; then
  echo "Built $out — it imports the engine from $seat/$engine$suffix"
else
  echo "Built $out — run it with: ./islands.sh --cartridge $bundle"
fi
