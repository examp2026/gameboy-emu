#include <rom.h>

uint8_t read(uint16_t address){
    return data[address];
}

void mock_load(){
    data.resize(65536);
    for(int i=16384; i<=32767; ++i)
        data[i] = 0x01;
    for(int i=32768; i<=49151; ++i)
        data[i] = 0x02;
    for(int i=49152; i<=65535; ++i)
        data[i] = 0x03;
}
