set -euo pipefail

RECIPES="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$RECIPES/../.." && pwd)"
NAME="$(basename "${BASH_SOURCE[1]}" .sh)"
PROFILE="${1:-}"
PLATFORM="${2:-}"
case "$PROFILE" in
  debug)   CMAKE_TYPE=Debug;   CFLAGS_PROFILE="-O0 -g" ;;
  release) CMAKE_TYPE=Release; CFLAGS_PROFILE="-O2" ;;
  *) echo "usage: libs/recipes/$NAME.sh <debug|release> [windows]" >&2; exit 2 ;;
esac
case "$PLATFORM" in
  "")
    ZONE="$PROFILE"
    TOOLCHAIN=()
    CC="${CC:-cc}"; AR=ar; RANLIB=ranlib; STRIP=strip
    PIC="-fPIC" ;;
  windows)
    ZONE="windows/$PROFILE"
    TOOLCHAIN=(-DCMAKE_TOOLCHAIN_FILE="$RECIPES/windows.cmake")
    CROSS=x86_64-w64-mingw32-
    CC="${CROSS}gcc"; AR="${CROSS}ar"; RANLIB="${CROSS}ranlib"; STRIP="${CROSS}strip"
    PIC="" ;;
  *) echo "usage: libs/recipes/$NAME.sh <debug|release> [windows]" >&2; exit 2 ;;
esac
HERE="$ROOT/libs/$ZONE/$NAME"
BUILD="$ROOT/build/libs/$ZONE/$NAME"
JOBS="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
mkdir -p "$HERE" "$BUILD"

finish() {
  if [[ "$PROFILE" == release ]]; then
    for artifact in "$@"; do
      [[ -f "$artifact" && ! -L "$artifact" ]] && "$STRIP" --strip-debug "$artifact"
    done
  fi
  echo "built ${*}"
}
