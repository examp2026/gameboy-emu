#include "../bus.h"
#include "../mbc.h"
#include <cassert>
#include <cstdint>
#include <iostream>

namespace {
struct TestEnv {
    static constexpr size_t BANK_SIZE{0x4000};
    static constexpr size_t TOTAL_BANKS{512};

    static std::vector<uint8_t> make_fake_rom() {
        std::vector<uint8_t> rom(TOTAL_BANKS * BANK_SIZE, 0x00);

        rom[0x148] = 0x08;
        rom[1 * BANK_SIZE] = 0xAA;
        rom[2 * BANK_SIZE] = 0xBB;
        rom[256 * BANK_SIZE] = 0xCC;

        return rom;
    }

    Sram sram;
    Cartridge cartridge;
    Bus bus;

    TestEnv() : sram(), cartridge(sram, make_fake_rom()), bus(cartridge) {}
};

} // namespace

//------------------------------------------------------------------------------

void test_bus_mbc_bank_switching() {
    TestEnv env;

    env.bus.write(0x2000, 0x02);
    env.bus.write(0x3000, 0x00);
    assert(env.bus.read(0x4000) == 0xBB);

    env.bus.write(0x2000, 0x00);
    env.bus.write(0x3000, 0x01);
    assert(env.bus.read(0x4000) == 0xCC);

    env.bus.write(0x2000, 0x01);
    env.bus.write(0x3000, 0x00);
    assert(env.bus.read(0x4000) == 0xAA);
}

//------------------------------------------------------------------------------

void test_bus_sram() {
    TestEnv env;

    env.bus.write(0x0000, 0xAA); // enables SRAM for writing
    env.bus.write(0xA000, 0xAB);
    env.bus.write(0xA500, 0xBC);
    env.bus.write(0xBFFF, 0xCD);

    assert(env.bus.read(0xA000) == 0xAB);
    assert(env.bus.read(0xA500) == 0xBC);
    assert(env.bus.read(0xBFFF) == 0xCD);

    uint8_t vram_before = env.bus.read(0x9FFF);
    uint8_t wram_before = env.bus.read(0xC000);

    env.bus.write(0xA000, 0xFF);
    env.bus.write(0xBFFF, 0xFF);

    assert(env.bus.read(0x9FFF) == vram_before);
    assert(env.bus.read(0xC000) == wram_before);
}

//------------------------------------------------------------------------------

void test_bus_vram_boundaries() {
    TestEnv env;

    env.bus.write(0x8000, 0xAB);
    env.bus.write(0x8FFF, 0xBC);
    env.bus.write(0x9FFF, 0xCD);

    assert(env.bus.read(0x8000) == 0xAB);
    assert(env.bus.read(0x8FFF) == 0xBC);
    assert(env.bus.read(0x9FFF) == 0xCD);

    uint8_t rom_before = env.bus.read(0x7FFF);
    uint8_t sram_before = env.bus.read(0xA000);

    env.bus.write(0x8000, 0xFF);
    env.bus.write(0x9FFF, 0xFF);

    assert(env.bus.read(0x7FFF) == rom_before);
    assert(env.bus.read(0xA000) == sram_before);
}

//------------------------------------------------------------------------------

void test_bus_wram_boundaries() {
    TestEnv env;

    env.bus.write(0xC000, 0xAB);
    env.bus.write(0xCFFF, 0xBC);
    env.bus.write(0xDFFF, 0xCD);

    assert(env.bus.read(0xC000) == 0xAB);
    assert(env.bus.read(0xCFFF) == 0xBC);
    assert(env.bus.read(0xDFFF) == 0xCD);

    uint8_t sram_before = env.bus.read(0xBFFF);

    env.bus.write(0xC000, 0xFF);
    assert(env.bus.read(0xBFFF) == sram_before);
}

//------------------------------------------------------------------------------

void test_bus_echo_ram_offset() {
    TestEnv env;

    env.bus.write(0xE001, 0xAB);
    env.bus.write(0xEFFF, 0xBC);
    env.bus.write(0xFDFF, 0xCD);

    assert(env.bus.read(0xC001) == 0xAB);
    assert(env.bus.read(0xCFFF) == 0xBC);
    assert(env.bus.read(0xDDFF) == 0xCD);
}

//------------------------------------------------------------------------------

void test_bus_echo_does_not_leaks_to_oam() {
    TestEnv env;

    uint8_t oam_before = env.bus.read(0xFE00);
    env.bus.write(0xFDFF, 0x44);
    assert(env.bus.read(0xFE00) == oam_before);
}

//------------------------------------------------------------------------------

void test_bus_echo_does_not_leaks_from_oam() {
    TestEnv env;

    uint8_t wram_before = env.bus.read(0xDE00); // FE00 - 2000 = DE00
    env.bus.write(0xFE00, 0x44);
    assert(env.bus.read(0xDE00) == wram_before);
}

//------------------------------------------------------------------------------

void test_bus_echo_ram_last_address_works() {
    TestEnv env;

    env.bus.write(0xFDFF, 0xAB);
    assert(env.bus.read(0xDDFF) == 0xAB);
}

//------------------------------------------------------------------------------

void test_bus_oam() {
    TestEnv env;

    env.bus.write(0xFE00, 0xAB);
    assert(env.bus.read(0xFE00) == 0xAB);

    env.bus.write(0xFE9F, 0xBC);
    assert(env.bus.read(0xFE9F) == 0xBC);
}

//------------------------------------------------------------------------------

void test_bus_hram() {
    TestEnv env;

    env.bus.write(0xFF80, 0xAB);
    assert(env.bus.read(0xFF80) == 0xAB);

    env.bus.write(0xFFFE, 0xBC);
    assert(env.bus.read(0xFFFE) == 0xBC);
}

//------------------------------------------------------------------------------

// void test_bus_t_cycles_for_read() {
//     TestEnv env;

//     uint8_t t_cycles_before = env.bus.get_t_cycles();

//     env.bus.read(0xC000);
//     assert(env.bus.get_t_cycles() == t_cycles_before);
// }

//------------------------------------------------------------------------------

// void test_bus_io()
// {
//     TestEnv env;
// }

// void test_bus_io_read()
// {

// }

// void test_bus_io_write()
// {

// }

int run_bus_tests() {
    test_bus_mbc_bank_switching();
    test_bus_sram();
    test_bus_vram_boundaries();
    test_bus_wram_boundaries();
    test_bus_echo_ram_offset();
    test_bus_echo_does_not_leaks_to_oam();
    test_bus_echo_does_not_leaks_from_oam();
    test_bus_echo_ram_last_address_works();
    test_bus_oam();
    test_bus_hram();
    return 0;
}
