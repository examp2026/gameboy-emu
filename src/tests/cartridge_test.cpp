#include "../cartridge.h"
#include "../mbc.h"
#include <iostream>
#include <cassert>

//------------------------------------------------------------------------------

void test_cartridge_write()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));

    cartridge.write(0x0000, 0x0A);
    
    cartridge.write(0x5FFF, 0x02);
    
    cartridge.write(0xA000, 0xAA);
    cartridge.write(0xA500, 0xBB);
    cartridge.write(0xBFFF, 0xCC);

    assert(cartridge.read(0xA000) == 0xAA);
    assert(cartridge.read(0xA500) == 0xBB);
    assert(cartridge.read(0xBFFF) == 0xCC);
}

//------------------------------------------------------------------------------

void test_cartridge_parse_header()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    fake_rom_bytes[0x147]  = 0x19;
    fake_rom_bytes[0x148]  = 0x08;
    fake_rom_bytes[0x149]  = 0x04;
    fake_rom_bytes[0x4000] = 0xAA;
    Cartridge cartridge(sram, std::move(fake_rom_bytes));

    assert((cartridge.read(0x4000) == 0xAA));
    
}


//------------------------------------------------------------------------------

int run_cartridge_tests()
{
    test_cartridge_parse_header();
    test_cartridge_write();
    return 0;
}
