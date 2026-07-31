cmake_minimum_required(VERSION 3.14)
project(gb_emulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -g -fsanitize=address,leak)
add_link_options(-fsanitize=address,leak)

include_directories(src)

set(SOURCE_FILES
    src/bus.cpp
    src/cartridge.cpp
    src/mbc.cpp
    src/rom.cpp
    src/sram.cpp
    src/cpu.cpp
)

add_executable(gb_emulator src/main.cpp ${SOURCE_FILES})

add_executable(global_tests
    src/tests/main_test.cpp
    src/tests/bus_test.cpp
    src/tests/cartridge_test.cpp
    src/tests/mbc_test.cpp
    src/tests/cpu_test.cpp
    ${SOURCE_FILES}
)
