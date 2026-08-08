#include "../cpu.h"
#include <cassert>

void test_cpu_set()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);
    
    cpu.setBC(0xBBAA);
    assert(cpu.getBC() == 0xBBAA);

    cpu.setDE(0xBBAA);
    assert(cpu.getDE() == 0xBBAA);

    cpu.setHL(0xBBAA);
    assert(cpu.getHL() == 0xBBAA);

    cpu.setAF(0xBBAA);
    assert(cpu.getAF() == 0xBBA0);

    cpu.setSP(0xAABB);
    assert(cpu.getSP() == 0xAABB);

    cpu.setPC(0xBBAA);
    assert(cpu.getPC() == 0xBBAA);

    assert(cpu.get_r8_dest(0xAA) == 0b00000101);
    assert(cpu.get_r8_source(0xAA) == 0b00000010);

    uint8_t dest = cpu.get_r16_dest(0xAA);
    assert(dest == 0b00000010);

    for(int i = 0; i <= 7; i++){
	cpu.set_r8(i, 0xAA);
	if(i != 6) assert(cpu.get_r8(i) == 0xAA);
    }

    cpu.setHL(0xC000);
    cpu.set_r8(0b110, 0xAA);
    assert(cpu.get_r8(0b110) == 0xAA);
    
    uint8_t stop = 0xFF;
}

//------------------------------------------------------------------------------

void test_cpu_fetch()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);

    bus.write(0x0000, 0x0A);
    bus.write(0xC000, 0xAA);
    cpu.setPC(0xC000);

    assert(cpu.fetch() == 0xAA);
    assert(cpu.getPC() == 0xC001);
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);

    uint16_t bytes {};

    cpu.setBC(0x4243);
    bytes = cpu.get_r16rp(0b00);
    assert(bytes == 0x4243);

    cpu.setDE(0x4445);
    bytes = cpu.get_r16rp(0b01);
    assert(bytes == 0x4445);

    cpu.setHL(0x484C);
    bytes = cpu.get_r16rp(0b10);
    assert(bytes == 0x484C);

    cpu.setSP(0x5350);
    bytes = cpu.get_r16rp(0b11);
    assert(bytes == 0x5350);
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp2()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);

    uint16_t bytes {};

    cpu.setBC(0x4243);
    bytes = cpu.get_r16rp2(0b00);
    assert(bytes == 0x4243);

    cpu.setDE(0x4445);
    bytes = cpu.get_r16rp2(0b01);
    assert(bytes == 0x4445);

    cpu.setHL(0x484C);
    bytes = cpu.get_r16rp2(0b10);
    assert(bytes == 0x484C);
    
    cpu.setAF(0x4146);
    bytes = cpu.get_r16rp2(0b11);
    assert(bytes == 0x4140);
}

//------------------------------------------------------------------------------

void test_cpu_get_r16mem()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);

    uint8_t source {};
    uint8_t byte {};

    bus.write(0xC000, 0xAA);
    cpu.setBC(0xC000);
    source = cpu.get_r16_dest(0b00000000);
    byte = cpu.get_r16mem(source);

    assert(byte == 0xAA);

    bus.write(0xC001, 0xAA);
    cpu.setDE(0xC001);
    source = cpu.get_r16_dest(0b00010000);
    byte = cpu.get_r16mem(source);

    assert(byte == 0xAA);

    bus.write(0xC002, 0xAA);
    cpu.setHL(0xC002);
    source = cpu.get_r16_dest(0b00100000);
    byte = cpu.get_r16mem(source);

    assert(byte == 0xAA);

    bus.write(0xC010, 0xAA);
    cpu.setAF(0xC010);
    source = cpu.get_r16_dest(0b00110000);
    byte = cpu.get_r16mem(source);

    assert(byte == 0xAA);
}

//------------------------------------------------------------------------------

void test_cpu_instructions()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);

    cpu.setBC(0xAABB);
    
    cpu.ld_r8_r8(0b000, 0b001);
    uint8_t r8_l = cpu.get_r8(0b000);
    uint8_t r8_r = cpu.get_r8(0b001);    
    assert(r8_l == r8_r);
}

//------------------------------------------------------------------------------

void test_cpu_decode()
{
    Sram sram;
    std::vector<uint8_t> fake_rom_bytes(0x8000, 0x00);
    Cartridge cartridge(sram, std::move(fake_rom_bytes));
    Bus bus(cartridge);
    CPU cpu(bus);

    bus.write(0x0000, 0x0A);
    //C -> B
    bus.write(0xC000, 0x41);
    cpu.setPC(0xC000);
    cpu.set_r8(0b000, 0xAA);
    cpu.set_r8(0b001, 0xBB);
    cpu.decode();

    assert(cpu.get_r8(0b000) == 0xBB);    
}

//------------------------------------------------------------------------------

int run_cpu_tests()
{
    test_cpu_set();
    test_cpu_fetch();
    test_cpu_decode();
    test_cpu_get_r16mem();
    test_cpu_get_r16rp();
    test_cpu_get_r16rp2();
    test_cpu_instructions();

    return 0;
}
