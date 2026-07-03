#pragma once
#include <cstdint>
#include "rom.h"
#include "sram.h"

class Mbc
{
 private:
    uint8_t ram_enable_flag {};
    uint8_t ram_bank {};
    uint16_t rom_bank {1};
    Sram& sram;
 public:
    Mbc(Sram& sram);
    void    write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

    uint16_t get_rom_bank() const;
    uint8_t  get_ram_bank() const;
    bool     is_ram_enabled() const;
};
