#pragma once
#include <vector>
#include <cstdint>

class Rom
{
 private:
    std::vector<uint8_t> data {};
 public:
    void read(uint16_t address);
    void mock_load();
};
