#include "../bus.h"
#include "../mbc.h"
#include <cassert>
#include <iostream>
#include <cstdint>

void test_bus_read()
{
    Sram sram;
    
    constexpr size_t BANK_SIZE = 0x4000;
    constexpr size_t TOTAL_BANKS = 512;
    
    std::vector<uint8_t> fake_rom_bytes(TOTAL_BANKS * BANK_SIZE, 0x00);

    fake_rom_bytes[0x148] = 0x08;
    fake_rom_bytes[1*BANK_SIZE]   = 0xAA;
    fake_rom_bytes[2*BANK_SIZE]   = 0xBB;
    fake_rom_bytes[256*BANK_SIZE] = 0xCC;

    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);

    bus.write(0x0000, 0x0A);
    assert(bus.read(0x4000) == 0xAA);
    
    bus.write(0x2000, 0x02); //lower_byte
    bus.write(0x3000, 0x00); //higher_bit
    assert(bus.read(0x4000) == 0xBB);

    bus.write(0x2000, 0x00);
    bus.write(0x3000, 0x01);
    assert(bus.read(0x4000) == 0xCC);
    
    bus.write(0xA000, 0xFF);
    assert(bus.read(0xA000) == 0xFF);
    
    bus.write(0xA500, 0xFF);
    assert(bus.read(0xA500) == 0xFF);
    
    bus.write(0xBFFF, 0xFF);
    assert(bus.read(0xBFFF) == 0xFF);

    bus.write(0xFF00, 0xAA);
    assert(bus.read(0xFF00) == 0xAA);

    bus.write(0xFF35, 0xBB);
    assert(bus.read(0xFF35) == 0xBB);

    bus.write(0xFF70, 0xCC);
    assert(bus.read(0xFF70) == 0xCC);

}


int run_bus_tests()
{
    test_bus_read();
    return 0;
}
