#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
SOURCE="$ROOT/submodules/mbedtls"
CROSSING=()
[[ "$PLATFORM" == windows ]] && CROSSING=(WINDOWS_BUILD=1 AR_DASH=)

make -C "$SOURCE" clean >/dev/null
make -C "$SOURCE" lib CC="$CC" AR="$AR" ${CROSSING[@]+"${CROSSING[@]}"} \
  CFLAGS="$CFLAGS_PROFILE $PIC" -j"$JOBS"
rm -f "$HERE/libmbedtls.a"
"$AR" -M <<SCRIPT
CREATE $HERE/libmbedtls.a
ADDLIB $SOURCE/library/libmbedtls.a
ADDLIB $SOURCE/library/libmbedx509.a
ADDLIB $SOURCE/library/libmbedcrypto.a
SAVE
END
SCRIPT
"$RANLIB" "$HERE/libmbedtls.a"
finish "$HERE/libmbedtls.a"
