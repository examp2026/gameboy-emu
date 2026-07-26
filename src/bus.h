#include "cartridge.h"
#include <array>
#include <cstdint>
#include <vector>

class Bus {
public:
    Bus(Cartridge& cartridge) : cartridge(cartridge){
	wram.resize(0x2000, 0x00);
	hram.resize(0x7F, 0x00);
    }
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);
private:
    Cartridge& cartridge;
    std::vector<uint8_t> wram;
    std::vector<uint8_t> hram;
};

