#include "cartridge.h"
#include <array>
#include <cstdint>
#include <vector>

class Bus {
public:
    Bus(Cartridge& cartridge) : cartridge(cartridge){
	vram.resize(0x2000, 0x00);
	wram.resize(0x2000, 0x00);
	oam.resize(0xA0, 0x00);
	hram.resize(0x7F, 0x00);
    }
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);
private:
    Cartridge& cartridge;
    std::vector<uint8_t> vram;
    std::vector<uint8_t> wram;
    std::vector<uint8_t>  oam;
    std::vector<uint8_t> hram;
    std::array<uint8_t, 128> io_registers;

    uint8_t io_read(uint16_t address);
    void io_write(uint16_t address, uint8_t value);
    
};

