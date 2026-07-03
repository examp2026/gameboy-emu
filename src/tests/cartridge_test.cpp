#include "../cartridge.h"
#include "../mbc.h"
#include <iostream>

Sram sram;
Mbc mbc(sram);
Cartridge cartridge();

int run_cartridge_tests(){
    std::cout << "cartridge hi";

    return 0;
}
