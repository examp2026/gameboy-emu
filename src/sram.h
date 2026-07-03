#pragma once
#include <vector>
#include <cstdint>

class Sram
{
 private:
    std::vector<uint8_t> data;
 public:
    uint8_t read(size_t index);
    void write(size_t index, uint8_t value);
    void mock_load();
};
