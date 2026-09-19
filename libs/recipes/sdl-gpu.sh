#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
SOURCE="$ROOT/submodules/SDL"

cmake -S "$SOURCE" -B "$BUILD" -DCMAKE_BUILD_TYPE=$CMAKE_TYPE \
  ${TOOLCHAIN[@]+"${TOOLCHAIN[@]}"} \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST_LIBRARY=OFF \
  -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF -DSDL_INSTALL_DOCS=OFF
cmake --build "$BUILD" --parallel "$JOBS"
cp "$BUILD/libSDL3.a" "$HERE/"
finish "$HERE/libSDL3.a"
