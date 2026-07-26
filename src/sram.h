#pragma once
#include <vector>
#include <cstdint>

class Sram
{
 public:
    Sram()
    {
	data.resize(4 * 8192);
    }
    uint8_t read(size_t index);
    void write(size_t index, uint8_t value);
    size_t size();
    void mock_load();    
 private:
    std::vector<uint8_t> data;
};
