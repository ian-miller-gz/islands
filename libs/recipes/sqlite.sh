#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
SOURCE="$ROOT/submodules/sqlite"

if [[ ! -f "$BUILD/sqlite3.c" ]]; then
  (cd "$BUILD" && "$SOURCE/configure" && make sqlite3.c)
fi
"$CC" $CFLAGS_PROFILE $PIC -DSQLITE_THREADSAFE=1 -DSQLITE_OMIT_LOAD_EXTENSION \
  -c "$BUILD/sqlite3.c" -o "$BUILD/sqlite3.o"
rm -f "$HERE/libsqlite3.a"
"$AR" rcs "$HERE/libsqlite3.a" "$BUILD/sqlite3.o"
"$RANLIB" "$HERE/libsqlite3.a"
mkdir -p "$HERE/include/sqlite"
cp "$BUILD/sqlite3.h" "$HERE/include/sqlite/"
finish "$HERE/libsqlite3.a"
