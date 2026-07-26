#include "rom.h"

uint8_t Rom::read(uint32_t real_index)
{
    if(real_index >= data.size()){
	return 0xFF;
    }	
    return data[real_index];
}

size_t Rom::size()
{
    size_t total_banks = data.size() / 0x4000;
    return total_banks;
}

void Rom::mock_load()
{
    data.resize(65536);
    for(int i=16384; i<=32767; ++i)
        data[i] = 0x01;
    for(int i=32768; i<=49151; ++i)
        data[i] = 0x02;
    for(int i=49152; i<=65535; ++i)
        data[i] = 0x03;
}
