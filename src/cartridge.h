#pragma once
#include <cstdint>
#include <memory>
#include "rom.h"
#include "sram.h"
#include "mbc.h"

class Cartridge
{
private:
    Rom  rom;
    Sram sram;    
    std::unique_ptr<Mbc> mbc;
 public:
    uint8_t read(uint16_t address);
};
