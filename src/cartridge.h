#pragma once
#include <cstdint>
#include <memory>
#include "rom.h"
#include "sram.h"
#include "mbc.h"

class Cartridge
{
public:
    Cartridge(Sram& sram, std::vector<uint8_t> raw_data);
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);
private:    
    Sram& sram;
    Rom rom;
    std::unique_ptr<Mbc> mbc;
    void parse_header();
};
