#include "bus.h"

//------------------------------------------------------------------------------

uint8_t Bus::read(uint16_t address)
{
    //-----[ ROM ranges ]-------------------------------------------------------
    
    if(address <= 0x7FFF){
	return cartridge.read(address);
    }
    
    //-----[ SRAM ranges ]------------------------------------------------------
    
    if(address >= 0xA000 && address <= 0xBFFF){
	return cartridge.read(address);
    }
    
    //-----[ WRAM ranges ]------------------------------------------------------
    
    if(address >= 0xC000 && address <= 0xDFFF){
	size_t offset = address - 0xC000;
	return wram.at(offset);
    }
    
    //-----[ VRAM ranges ]------------------------------------------------------
    
    if(address >= 0x8000 && address <= 0x9FFF){
	size_t offset = address - 0x8000;
	return vram.at(offset);
    }
    
    //-----[ Echo RAM ranges ]--------------------------------------------------
    
    if(address >= 0xE000 && address <= 0xFDFF){
	size_t wram_mirror_offset = address & ~0x2000;
	size_t offset = wram_mirror_offset - 0xC000;
	uint8_t echo_ram = wram.at(offset);
	return echo_ram;
    }
    
    //-----[ OAM ranges ]-------------------------------------------------------
    
    if(address >= 0xFE00 && address <= 0xFE9F){
	size_t offset = address - 0xFE00;
	return oam.at(offset);
    }

    //-----[ HRAM ranges ]------------------------------------------------------

    if(address >= 0xFF80 && address <= 0xFFFE){
	size_t offset = address - 0xFF80;
	return hram.at(offset);
    }
    
    //-----[ IO ranges ]--------------------------------------------------------
    
    if(address >= 0xFF00 && address <= 0xFF7F){
	return io_read(address);
    }

    //-----[ Not Usable ]-------------------------------------------------------
    
    if(address >= 0xFEA0 && address <= 0xFEFF){
	return 0xFF;
    }

    //--------------------------------------------------------------------------

    if(address == 0xFFFF){
	//ie_register
    }

    //--------------------------------------------------------------------------

    return 0xFF;
}

//------------------------------------------------------------------------------

void Bus::write(uint16_t address, uint8_t value)
{
    //-----[ ROM ranges ]-------------------------------------------------------
    
    if(address <= 0x5FFF){
	cartridge.write(address, value);
	return;
    }

    //-----[ SRAM ranges ]------------------------------------------------------
    
    if(address >= 0xA000 && address <= 0xBFFF){
	cartridge.write(address, value);
	return;
    }

    //-----[ WRAM ranges ]------------------------------------------------------
    
    if(address >= 0xC000 && address <= 0xDFFF){
	size_t offset = address - 0xC000;
	wram.at(offset) = value;
	return;
    }

    //-----[ VRAM ranges ]------------------------------------------------------
    
    if(address >= 0x8000 && address <= 0x9FFF){
	size_t offset = address - 0x8000;
	vram.at(offset) = value;
	return;
    }

    //-----[ Echo RAM ranges ]--------------------------------------------------

    if(address >= 0xE000 && address <= 0xFDFF){

	size_t wram_mirror_offset = address & ~0x2000;
	size_t offset = wram_mirror_offset - 0xC000;
	wram.at(offset) = value;
	return;
    }

    //-----[ OAM ranges ]-------------------------------------------------------

    if(address >= 0xFE00 && address <= 0xFE9F){
	size_t offset = address - 0xFE00;
	oam.at(offset) = value;
	return;
    }

    //-----[ HRAM ranges ]------------------------------------------------------
    
    if(address >= 0xFF80 && address <= 0xFFFE){
	size_t offset = address - 0xFF80;
	hram.at(offset) = value;
	return;
    }

    //-----[ IO ranges ]--------------------------------------------------------
    
    if(address >= 0xFF00 && address <= 0xFF70){
	io_write(address, value);
	return;
    }

    return;
}

//------------------------------------------------------------------------------

uint8_t Bus::io_read(uint16_t address)
{
    size_t offset = address - 0xFF00;    
    return io_registers.at(offset);
}

//------------------------------------------------------------------------------

void Bus::io_write(uint16_t address, uint8_t value)
{
    size_t offset = address - 0xFF00;
    io_registers.at(offset) = value;
}

//------------------------------------------------------------------------------
