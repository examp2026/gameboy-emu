#include "sram.h"

uint8_t Sram::read(size_t real_index)
{
    if(real_index >= data.size()){
	return 0xFF;
    }
    
    return data[real_index];
}

void Sram::write(size_t real_index, uint8_t value){
    if(real_index >= data.size()){
	return;
    }
    
    data[real_index] = value;
}

size_t Sram::size()
{
    size_t total_banks = data.size() / 0x2000;
    return total_banks;
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
