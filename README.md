# lsd

A C++17 command-line utility that prints a directory's contents recursively,
with optional file-type and size details. macOS and Linux are supported by the
platform-specific clipboard integration.

## Build

With CMake 3.10 or later and a C++17 compiler:

```sh
cmake -S . -B build
cmake --build build
```

Alternatively, build directly without CMake:

```sh
mkdir -p build
c++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o build/lsd
```

## Usage

```sh
./build/lsd                   # current directory
./build/lsd src               # explicit directory
./build/lsd / 0 src           # list only src's immediate entries
./build/lsd - + src           # include hidden entries and show details
./build/lsd h                 # help
```

The parser currently accepts the literal option tokens `/` (followed by a
recursion depth), `-` (hidden entries), `+` (details), `=` (clipboard), and `h`
(help). Despite the help text, long options are not implemented. Depth `0`
lists immediate entries; the default `-1` recursively lists all levels.
Use `/.` rather than `/` to select the filesystem root, since `/` is an option.

Clipboard copying uses `pbcopy` on macOS or `xclip -selection clipboard` on
Linux; the relevant utility must be installed. Directory traversal follows
links to directories, so avoid trees containing symbolic-link cycles.

`bash install.sh` builds and copies the executable to `/usr/local/bin`.
It requires CMake, Make, and write access to that directory, and removes the
repository's `build` directory. Prefer the local build above when exploring.

## Regression tests

Python 3 is needed only for tests:

```sh
python3 tests/test_cli.py ./build/lsd
```

Tests use temporary directories and do not invoke clipboard or installation.
