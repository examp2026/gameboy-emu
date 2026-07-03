#include <cassert>
#include <iostream>
#include "../mbc.h"

void test_ram_enable()
{
    Sram sram;
    Mbc mbc(sram);

    assert(mbc.is_ram_enabled() == false);

    mbc.write(0x1000, 0x0A);
    assert(mbc.is_ram_enabled() == true);

    mbc.write(0x1500, 0x55);
    assert(mbc.is_ram_enabled() == false);
}

void test_ram_banking()
{
    Sram sram;
    Mbc mbc(sram);

    mbc.write(0x2000, 0x8F);
    assert(mbc.get_rom_bank() == 0x8F);

    mbc.write(0x3000, 0x9F);
    assert(mbc.get_rom_bank() == 0x18F);
}

void test_ram_banking_truncation()
{
    Sram sram;
    Mbc mbc(sram);

    mbc.write(0x4500, 0x55);
    assert(mbc.get_ram_bank() == 0x05);
}

int run_mbc_tests()
{
    test_ram_enable();
    test_ram_banking();
    test_ram_banking_truncation();

    std::cout << "MBC tests passed." << std::endl;
    return 0;
}
