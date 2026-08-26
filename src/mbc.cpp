#include "mbc.h"
#include <cstdint>
#include <iostream>

//------------------------------------------------------------------------------

Mbc::Mbc(
    uint8_t rom_code,
    uint8_t sram_code,
    size_t rom_size,
    size_t sram_size
    )
    : ram_enable_flag(false), sram_bank(0), rom_bank(1) {
    rom_banks_max = (2 << rom_code);
    rom_total_banks = rom_size;
    sram_total_banks = sram_size;

    switch (sram_code) {
    case 0x00:
        sram_banks_max = 0;
        break;
    case 0x01:
        sram_banks_max = 1;
        break;
    case 0x02:
        sram_banks_max = 1;
        break;
    case 0x03:
        sram_banks_max = 4;
        break;
    case 0x04:
        sram_banks_max = 16;
        break;
    default:
        sram_banks_max = 0;
        break;
    }
}

//------------------------------------------------------------------------------

void Mbc::write(uint16_t address, uint8_t value) {
    if (address <= 0x1FFF) {
        uint8_t lower_nibble = value & 0x0F;
        if (lower_nibble == 0x0A) {
            ram_enable_flag = true;
        } else {
            ram_enable_flag = false;
        }
        //      ram_enable_flag = ((value & 0x0F) == 0x0A);
    }
    if (address >= 0x2000 && address <= 0x2FFF) {
        rom_bank = (rom_bank & 0x0100) | (value & 0x00FF);
    }
    if (address >= 0x3000 && address <= 0x3FFF) {
        rom_bank =
	    (static_cast<uint16_t>(value & 0x01) << 8) | (rom_bank & 0x00FF);
    }
    // Max - 8KiB * 16 banks
    if (address >= 0x4000 && address <= 0x5FFF) {
        sram_bank = value & 0x0F;
    }
}

//------------------------------------------------------------------------------

uint32_t Mbc::map_address(uint16_t address) {
    if (address <= 0x3FFF) {
        return address;
    }
    if (address >= 0x4000 && address <= 0x7FFF) {
        uint32_t actual_bank =
	    static_cast<uint32_t>(rom_bank & (rom_total_banks - 1));
        uint32_t real_index = (actual_bank * 16384) + (address - 0x4000);
        return real_index;
    }
    if (address >= 0xA000 && address <= 0xBFFF) {
        if (!ram_enable_flag) {
            return 0xFFFFFFFF;
        }
        uint32_t actual_bank =
	    static_cast<uint32_t>(sram_bank & (sram_total_banks - 1));
        uint32_t real_index = (address - 0xA000) + (actual_bank * 8192);
        return real_index;
    }
    return 0;
}

//------------------------------------------------------------------------------

uint16_t Mbc::get_rom_bank() const { return rom_bank; }

//------------------------------------------------------------------------------

uint8_t Mbc::get_ram_bank() const { return sram_bank; }

//------------------------------------------------------------------------------

bool Mbc::is_ram_enabled() const { return ram_enable_flag; }
