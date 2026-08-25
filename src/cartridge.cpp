// -*- mode: c++; c-file-style: "stroustrup"; c-basic-offset: 4; -*-

#include "cartridge.h"

Cartridge::Cartridge(Sram &sram, std::vector<uint8_t> raw_data)
    : sram(sram), rom(std::move(raw_data)) {
    mbc = std::make_unique<Mbc>(rom.read(0x148), rom.read(0x149), rom.size(), sram.size());
}

//------------------------------------------------------------------------------

uint8_t Cartridge::read(uint16_t address) {
    if (address <= 0x7FFF) {
        uint32_t real_index = mbc->map_address(address);
        return rom.read(real_index);
    }

    if (address >= 0xA000 && address <= 0xBFFF) {
        uint32_t real_index = mbc->map_address(address);
        return sram.read(real_index);
    }

    return 0;
}

//------------------------------------------------------------------------------

void Cartridge::parse_header() {
    // uint8_t mapper_id = rom.read(0x0147);
    uint8_t rom_code = rom.read(0x148);
    uint8_t ram_code = rom.read(0x149);

    Mbc mbc(rom_code, ram_code, rom.size(), sram.size());
}

//------------------------------------------------------------------------------

void Cartridge::write(uint16_t address, uint8_t value) {
    if (address <= 0x5FFF) {
        mbc->write(address, value);
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        if (mbc->is_ram_enabled() == true) {
            auto real_index = mbc->map_address(address);
            sram.write(real_index, value);
        }
    }
}
