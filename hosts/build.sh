#!/usr/bin/env bash
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(dirname "$here")"
bundle="${1:?usage: hosts/build.sh <host-dir>}"
cd "$root"

bundle="${bundle%/}"
name="$(basename "$bundle")"
[ -d "$bundle" ] || { echo "No host directory at $bundle" >&2; exit 1; }

seam="src/cartridge src/common src/logger src/metrics src/network"
[ -f "$bundle/links" ] && seam="$(cat "$bundle/links")"
links="${HOST_LINKS:-$seam}"

mapfile -t sources < <(find "$bundle" -name '*.cpp' | sort)
[ "${#sources[@]}" -gt 0 ] || { echo "No .cpp files under $bundle" >&2; exit 1; }

zone=""
if measured="$(python3 -c 'from make import api; print(api.zone())' 2>/dev/null)"; then
  [ -d "build/objects/$measured" ] && zone="build/objects/$measured"
fi

for candidate in $(ls -1dt build/objects/*/cache.yaml 2>/dev/null); do
  [ -z "$zone" ] || break
  candidate="$(dirname "$candidate")"
  sample="$(find "${candidate%/}" -name 'wip.*.o' -print -quit 2>/dev/null)"
  [ -n "$sample" ] || continue
  file -b "$sample" | grep -q '^ELF' || continue
  loader="${candidate%/}/src/cartridge/wip.load.dynamic.o"
  [ -f "$loader" ] || continue
  nm --defined-only "$loader" 2>/dev/null | grep -q 'ZN9CARTRIDGE4path' || continue
  zone="${candidate%/}"
  break
done
[ -n "$zone" ] || { echo "No linkable engine build under build/objects — run 'python3 -m make' first" >&2; exit 1; }
archived=""
if roots="$(python3 - <<'PYEOF' 2>/dev/null
from make import api
outputs = api.CONFIG.BUILD_OUTPUT_PATH
for archive in api.CONFIG.LIBRARY_ARCHIVES:
    try:
        print(archive.parent.relative_to(outputs))
    except ValueError:
        pass
PYEOF
)"; then archived="$roots"; fi

walled() {
  local path="$1/" root
  for root in $archived; do
    case "$path" in "$root"/*) return 0 ;; esac
  done
  return 1
}

objects=()
stale=0
walls=0
for subtree in $links; do
  while IFS= read -r object; do
    rel="${object#"$zone/"}"
    dir="${rel%/*}"
    stem="${rel##*/wip.}"
    stem="${stem%.o}"
    if walled "$dir"; then
      walls=$((walls + 1))
    elif [ -f "$dir/$stem.cpp" ] || [ -f "$dir/$stem.c" ]; then
      objects+=("$object")
    else
      stale=$((stale + 1))
    fi
  done < <(find "$zone/$subtree" -name 'wip.*.o' 2>/dev/null | sort)
done
[ "$stale" -eq 0 ] ||
  echo "Skipping $stale stale object(s) under $zone — their sources are gone."
[ "$walls" -eq 0 ] ||
  echo "Skipping $walls archive-only object(s) — a host links the archive's"\
       "consumers, never its members."
[ "${#objects[@]}" -gt 0 ] || { echo "No engine objects for [$links] under $zone (build the engine first)" >&2; exit 1; }

isys=()
for d in $(find src -name '*.hpp' -exec dirname {} \; | sort -u) \
         src src/core cartridges/.core cartridges/.test cartridges; do
  isys+=(-isystem "$d")
done
iinc=(-I build/include)
for d in $(sed -n '/^includes:/,/^[a-z]/s/^[[:space:]]*-[[:space:]]*//p' \
             configs/make.yaml); do
  iinc+=(-I "$d")
done

zone="libs/$(sed -n 's/^[[:space:]]*SR_PROFILE:[[:space:]]*SR_\([A-Z]*\).*/\1/p' configs/make.yaml |
  head -1 | tr '[:upper:]' '[:lower:]')"
prune=()
for other in libs/debug libs/release; do
  [ "$other" = "$zone" ] || prune+=(-path "$other" -prune -o)
done
libflags=()
while IFS= read -r lib; do
  dir="$(dirname "$lib")"
  base="$(basename "$lib")"
  short="${base#lib}"; short="${short%%.*}"
  libflags+=("-L$dir" "-Wl,-rpath=$dir" "-l$short")
done < <(find libs -path libs/sdk -prune -o "${prune[@]}" \
           \( -name '*.so' -o -name '*.a' -o -name '*.so.*' \) -print | sort)

out="$bundle/$name.out"
echo "Building $out from ${#sources[@]} source file(s), ${#objects[@]} engine object(s)..."
g++ -std=c++23 "${isys[@]}" "${iinc[@]}" \
    -Wl,--allow-multiple-definition -Wl,--export-dynamic \
    "${sources[@]}" "${objects[@]}" "${libflags[@]}" -o "$out"
echo "Built $out — run it from the repo root: ./$out"
