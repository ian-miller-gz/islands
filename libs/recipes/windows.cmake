# The CMake side of the recipes' `windows` platform word (common.sh): a
# cross build for Windows 11 from a Linux desk, or a native one under MSYS2
# UCRT64, with MinGW-w64 under its canonical spelling — the same names
# configs/make.yaml's windows toolchain compiles and archives with. Nothing
# is searched for on the host: the find-root modes keep every library and
# header lookup off this machine's Linux packages, and a program (a code
# generator a tree runs while building) resolves from PATH as before.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
find_program(CMAKE_AR x86_64-w64-mingw32-ar)
find_program(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
