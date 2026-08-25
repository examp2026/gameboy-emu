#include "../mbc.h"
#include <cassert>
#include <cstdint>
#include <iostream>

void test_mbc_write() {}

//------------------------------------------------------------------------------

void test_ram_flag_enable() {
    Mbc mbc(0x00, 0x00, 0x02, 0x04);
    assert(mbc.map_address(0xA000) == 0xFFFFFFFF);
    assert(mbc.map_address(0xBFFF) == 0xFFFFFFFF);

    mbc.write(0x0000, 0x0A);
    assert(mbc.map_address(0xA000) == 0x0000);

    mbc.write(0x0000, 0x00);

    mbc.write(0x1FFF, 0x0A);
    assert(mbc.map_address(0xBFFF) == 0x1FFF);
}

//------------------------------------------------------------------------------

void test_update_rom_bank_low_byte() {
    Mbc mbc(0x00, 0x00, 0x02, 0x04);
    uint8_t expected_bank = 0;
    uint32_t bank_size = 0x4000;

    assert(mbc.map_address(0x0000) == 0x0000);
    assert(mbc.map_address(0x3FFF) == 0x3FFF);

    uint8_t written_bank = 5;
    expected_bank = 1;
    mbc.write(0x2000, written_bank);
    assert(mbc.map_address(0x4000) == (expected_bank * bank_size));
    assert(mbc.map_address(0x7FFF) == (expected_bank * bank_size) + 0x3FFF);

    expected_bank = 1;
    mbc.write(0x2000, expected_bank);
    assert(mbc.map_address(0x4000) == (expected_bank * bank_size));
}

//------------------------------------------------------------------------------

void test_rom_bank_high_bit() {
    Mbc mbc(0x00, 0x00, 0x200, 0x04);
    mbc.write(0x2000, 0x01);
    mbc.write(0x3000, 0x01);

    uint32_t expected_bank = 257;
    uint32_t bank_size = 16384;

    assert(mbc.map_address(0x4000) == (expected_bank * bank_size));
}

//------------------------------------------------------------------------------

void test_rom_mapping_default_bank() {
    Mbc mbc(0x00, 0x00, 0x02, 0x04);
    assert(mbc.map_address(0x4000) == 0x4000);
}

//------------------------------------------------------------------------------

void test_sram_bank_map_address() {
    Mbc mbc(0x00, 0x00, 0x02, 0x04);
    uint8_t written_bank = 5;
    uint8_t expected_bank = 0;
    uint32_t bank_size = 0x2000;

    mbc.write(0xA000, written_bank);
    assert(mbc.map_address(0xA000) == (expected_bank * bank_size));
    mbc.write(0xBFFF, written_bank);
    assert(mbc.map_address(0xBFFF) == (expected_bank * bank_size));

    expected_bank = 3;
    mbc.write(0xA000, expected_bank);
    assert(mbc.map_address(0xA000) == (expected_bank * bank_size));
    expected_bank = 2;
    mbc.write(0xAFFF, expected_bank);
    assert(mbc.map_address(0xAFFF) == (expected_bank * bank_size));
    expected_bank = 1;
    mbc.write(0xBFFF, expected_bank);
    assert(mbc.map_address(0xBFFF) == (expected_bank * bank_size));
}

//------------------------------------------------------------------------------

int run_mbc_tests() {
    test_ram_flag_enable();
    test_update_rom_bank_low_byte();
    test_rom_bank_high_bit();
    test_rom_mapping_default_bank();

    return 0;
}
