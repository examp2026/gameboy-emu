#include "sram.h"

uint8_t Sram::read(size_t index)
{
    return data[index];
}

void Sram::write(size_t index, uint8_t value){
    data[index] = value;
}

void Sram::mock_load()
{
    data.resize(4 * 8192);

    for(int i=8192; i<16384; ++i){
        data[i] = 0x11;
    }
    for(int i=16384; i<24575; ++i){
        data[i] = 0x22;
    }
    for(int i=24576; i<32768; ++i){
        data[i] = 0x33;
    }
}
