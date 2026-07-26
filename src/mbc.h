#pragma once
#include <cstdint>
#include "rom.h"
#include "sram.h"

class Mbc
{
public:
    Mbc(uint8_t rom_code, uint8_t sram_code, size_t rom_size, size_t sram_size);
    
    void     write(uint16_t address, uint8_t value);
    uint32_t map_address(uint16_t address);

    uint16_t get_rom_bank() const;
    uint8_t  get_ram_bank() const;
    bool     is_ram_enabled() const;
    
private:
    uint8_t ram_enable_flag {};
    uint8_t sram_bank {};
    uint16_t rom_bank {1};
    uint8_t sram_banks_max {};    
    uint8_t rom_banks_max {};
    size_t rom_total_banks {};
    size_t sram_total_banks {};
};
