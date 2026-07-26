#include "bus.h"

uint8_t Bus::read(uint16_t address)
{
    if(address <= 0x7FFF){
	return cartridge.read(address);
    }

    if(address >= 0xA000 && address <= 0xBFFF){
	return cartridge.read(address);
    }
    
    if(address >= 0xC000 && address <= 0xDFFF){
	size_t offset = address - 0xC000;
	return wram.at(offset);
    }

    return 0xFF;
}

void Bus::write(uint16_t address, uint8_t value)
{
    if(address <= 0x5FFF){
	cartridge.write(address, value);
    }

    if(address >= 0xA000 && address <= 0xBFFF){
	cartridge.write(address, value);
    }
    
    if(address >= 0xC000 && address <= 0xDFFF){
	size_t offset = address - 0xC000;
	wram.at(offset) = value;
    }

    return;
}
