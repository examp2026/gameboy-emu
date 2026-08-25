#pragma once
#include <cstdint>
#include <vector>

class Rom {
  public:
    explicit Rom(std::vector<uint8_t> rom_data) : data(std::move(rom_data)) {}
    uint8_t read(uint32_t real_index);
    size_t size();
    void mock_load();

  private:
    std::vector<uint8_t> data{};
};
