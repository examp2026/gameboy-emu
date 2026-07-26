# Game boy emulator

A Game boy emulator written in C++.

## Current progress: 
**Mbc5**: `map_address()`, `write()` 
**Cartridge**: `read()`, `write()`, `parse_header()`
**Bus**: `read()`, `write()`

## Stack:
C++ 17/20
Cmake
Linux/WSL(Ubuntu)
GDB, ASan/LSan

How to build/run:
```bash
mkdir build && cd build
cmake ..
make
./global_tests
```