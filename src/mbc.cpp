#include <cstdint>
#include <iostream>
#include "mbc.h"

Mbc::Mbc(Sram& memory) : sram(memory)
{
    rom_bank = 1;
    ram_bank = 0;
    ram_enable_flag = false;
}

void Mbc::write(uint16_t address, uint8_t value)
{
    std::cerr << "[DEBUG]: value is 0x" << std::hex
              << static_cast<int>(value) << '\n';

    if(address >= 0x0000 && address <= 0x1FFF){
        ram_enable_flag = ((value & 0x0F) == 0x0A);
    }
    if(address >= 0x2000 && address <= 0x2FFF){
        std::cerr << "[DEBUG]: 0x2000; value is 0x" << std::hex
                  << static_cast<int>(value) << '\n';

        rom_bank = (rom_bank & 0xFF00) | (value & 0x00FF);
    }
    if(address >= 0x3000 && address <= 0x3FFF){
        std::cerr << "[DEBUG]: 0x3000; value is 0x" << std::hex
                  << static_cast<int>(value) << '\n';
        rom_bank = ((value & 0x01) << 8) | (rom_bank & 0x00FF );
    }
    if(address >= 0x4000 && address <= 0x5FFF){
        ram_bank = value & 0x0F;
    }
    if(address >= 0xA000 && address <= 0xBFFF){
        if(!ram_enable_flag){
            return;
        }
        uint32_t real_index = (address - 0xA000) + (ram_bank * 8192);
        sram.write(real_index, value);
    }
}

uint8_t Mbc::read(uint16_t address)
{
    if(address >= 0x0000 && address <= 0x3FFF){
        return address;
    }
    if(address >= 0x4000 && address <= 0x7FFF){
        uint32_t real_index = (rom_bank * 16384)  + (address - 0x4000);
        return real_index;
    }
    if(address >= 0xA000 && address <= 0xBFFF){
        if(!ram_enable_flag){
            return 0xFF;
        }
        uint32_t real_index = (address - 0xA000) + (ram_bank * 8192);
        return sram.read(real_index);
    }

    return 0;
}

uint16_t Mbc::get_rom_bank() const
{
    return rom_bank;
}

uint8_t Mbc::get_ram_bank() const
{
    return ram_bank;
}

bool Mbc::is_ram_enabled() const
{
    return ram_enable_flag;
}

