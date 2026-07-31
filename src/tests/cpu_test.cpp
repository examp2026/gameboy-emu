#include "../cpu.h"
#include <cassert>

void test_cpu_set()
{
    CPU cpu;

    cpu.setA(0xAA);
    assert(cpu.getA() == 0xAA);
    cpu.setB(0xAA);
    assert(cpu.getB() == 0xAA);
    cpu.setC(0xBB);
    assert(cpu.getC() == 0xBB);
    cpu.setD(0xAA);
    assert(cpu.getD() == 0xAA);
    cpu.setE(0xBB);
    assert(cpu.getE() == 0xBB);
    cpu.setH(0xAA);
    assert(cpu.getH() == 0xAA);
    cpu.setL(0xBB);
    assert(cpu.getL() == 0xBB);
    cpu.setF(0xBB);
    assert(cpu.getF() == 0xB0);

    cpu.setBC(0xBBAA);
    assert(cpu.getBC() == 0xBBAA);

    cpu.setDE(0xBBAA);
    assert(cpu.getDE() == 0xBBAA);

    cpu.setHL(0xBBAA);
    assert(cpu.getHL() == 0xBBAA);

    cpu.setAF(0xBBAA);
    assert(cpu.getAF() == 0xBBA0);

    cpu.setSP(0xAABB);
    assert(cpu.getSP() == 0xAABB);

    cpu.setPC(0xBBAA);
    assert(cpu.getPC() == 0xBBAA);

    uint8_t stop = 0xFF;
}

int run_cpu_tests()
{
    test_cpu_set();

    return 0;
}
