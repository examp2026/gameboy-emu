#include <array>
#include <cstdint>
#include <vector>

class Bus {
    
};


class Cartridge
{
    std::array<uint8_t, 16000> ROM_BANK_0 {};
    std::vector<uint8_t> ROM_BANK_N {};
    std::vector<uint8_t> ERAM {};
};
