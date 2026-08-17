#include "../cpu.h"
#include <cassert>

struct TestEnv
{
    Sram sram;
    Cartridge cartridge;
    Bus bus;
    CPU cpu;

    TestEnv()
	: cartridge(sram, std::vector<uint8_t>(0x8000, 0x00))
	, bus(cartridge)
	, cpu(bus) {}
};

//------------------------------------------------------------------------------

void test_cpu_register_pairs()
{
    TestEnv env;
    
    env.cpu.setBC(0xBBAA);
    assert(env.cpu.getBC() == 0xBBAA);

    env.cpu.setDE(0xBBAA);
    assert(env.cpu.getDE() == 0xBBAA);

    env.cpu.setHL(0xBBAA);
    assert(env.cpu.getHL() == 0xBBAA);

    env.cpu.setAF(0xBBAA);
    assert(env.cpu.getAF() == 0xBBA0);

    env.cpu.setSP(0xAABB);
    assert(env.cpu.getSP() == 0xAABB);

    env.cpu.setPC(0xBBAA);
    assert(env.cpu.getPC() == 0xBBAA);    
}

//------------------------------------------------------------------------------

void test_cpu_opcode_field_decoding()
{
  // get_r8_dest, get_r8_source, get_r16_dest

    TestEnv env;
    
    assert(env.cpu.decode_r8_dest(0xAA) == 0b101);
    assert(env.cpu.decode_r8_source(0xAA) == 0b010);
    assert(env.cpu.decode_r16_dest(0xAA) == 0b010);

    assert(env.cpu.decode_r8_dest(0x00) == 0b000);
    assert(env.cpu.decode_r8_source(0x00) == 0b000);
    assert(env.cpu.decode_r16_dest(0x00) == 0b00);

    assert(env.cpu.decode_r8_dest(0xFF) == 0b111);
    assert(env.cpu.decode_r8_source(0xFF) == 0b111);
    assert(env.cpu.decode_r16_dest(0xFF) == 0b11);
    
}

//------------------------------------------------------------------------------

void test_cpu_get_r8_generic_access()
{
    TestEnv env;

    //set_r8(), get_r8()
    for(int i = 0; i <= 7; i++){
	env.cpu.set_r8(i, 0xAA);
	//6(0b110) [HL] pair register must be written to memory, not to register
	if(i != 6) assert(env.cpu.get_r8(i) == 0xAA);
    }
    env.cpu.setHL(0xC000);
    env.cpu.set_r8(0b110, 0xAA);
    assert(env.bus.read(0xC000) == 0xAA);    
}

//------------------------------------------------------------------------------

void test_cpu_get_n8n16()
{
    TestEnv env;
    
    env.bus.write(0x0000, 0x0A);
    env.bus.write(0xC001, 0xAA);
    env.cpu.setPC(0xC000);

    uint8_t byte = env.cpu.get_n8();
    assert(byte == 0xAA);

    env.bus.write(0xC002, 0xBB);
    uint16_t bytes = env.cpu.get_n16();
    assert(bytes == 0xBBAA);
}


//------------------------------------------------------------------------------

void test_cpu_fetch()
{
    TestEnv env;
    
    env.bus.write(0x0000, 0x0A);
    env.bus.write(0xC000, 0xAA);
    env.cpu.setPC(0xC000);

    assert(env.cpu.fetch() == 0xAA);
    assert(env.cpu.getPC() == 0xC001);
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp()
{
    TestEnv env;
    
    uint16_t bytes {};

    env.cpu.setBC(0x4243);
    bytes = env.cpu.get_r16rp(0b00);
    assert(bytes == 0x4243);

    env.cpu.setDE(0x4445);
    bytes = env.cpu.get_r16rp(0b01);
    assert(bytes == 0x4445);

    env.cpu.setHL(0x484C);
    bytes = env.cpu.get_r16rp(0b10);
    assert(bytes == 0x484C);

    env.cpu.setSP(0x5350);
    bytes = env.cpu.get_r16rp(0b11);
    assert(bytes == 0x5350);
}

//------------------------------------------------------------------------------

void test_cpu_set_r16rp()
{
    TestEnv env;
    
    env.cpu.set_r16rp(0b00, 0x4243);
    assert(0x4243 == env.cpu.getBC());
    
    env.cpu.set_r16rp(0b01, 0x4445);
    assert(0x4445 == env.cpu.getDE());

    env.cpu.set_r16rp(0b10, 0x484C);
    assert(0x484C == env.cpu.getHL());

    env.cpu.set_r16rp(0b11, 0x5350);
    assert(0x5350 == env.cpu.getSP());
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp2()
{
    TestEnv env;
    
    uint16_t bytes {};

    env.cpu.setBC(0x4243);
    bytes = env.cpu.get_r16rp2(0b00);
    assert(bytes == 0x4243);

    env.cpu.setDE(0x4445);
    bytes = env.cpu.get_r16rp2(0b01);
    assert(bytes == 0x4445);

    env.cpu.setHL(0x484C);
    bytes = env.cpu.get_r16rp2(0b10);
    assert(bytes == 0x484C);
    
    env.cpu.setAF(0x4146);
    bytes = env.cpu.get_r16rp2(0b11);
    assert(bytes == 0x4140);
}

//------------------------------------------------------------------------------

void test_cpu_set_r16rp2()
{
    TestEnv env;

    env.cpu.set_r16rp2(0b00,0x4243);
    assert(0x4243 == env.cpu.getBC());

    env.cpu.set_r16rp2(0b01, 0x4445);
    assert(0x4445 == env.cpu.getDE());

    env.cpu.set_r16rp2(0b10, 0x484C);
    assert(0x484C == env.cpu.getHL());

    env.cpu.set_r16rp2(0b11, 0x4146);
    assert(0x4140 == env.cpu.getAF());
}

//------------------------------------------------------------------------------

void test_cpu_get_r16mem()
{
    TestEnv env;

    uint8_t byte {};

    env.bus.write(0xC000, 0xAA);
    env.cpu.setBC(0xC000);
    byte = env.cpu.get_r16mem(0b00);

    assert(byte == 0xAA);

    env.bus.write(0xC001, 0xBB);
    env.cpu.setDE(0xC001);
    byte = env.cpu.get_r16mem(0b01);

    assert(byte == 0xBB);

    env.bus.write(0xC002, 0xCC);
    env.cpu.setHL(0xC002);
    byte = env.cpu.get_r16mem(0b10);

    assert(byte == 0xCC);
    assert(env.cpu.getHL() == 0xC003);

    env.bus.write(0xC003, 0xDD);
    byte = env.cpu.get_r16mem(0b11);    

    assert(byte == 0xDD);
    assert(env.cpu.getHL() == 0xC002);
}

//------------------------------------------------------------------------------

void test_cpu_instructions_load()
{
    TestEnv env;

    //-----[ regular ld_r8_r8() ]-----------------------------------------------
    
    // env.cpu.setBC(0xAABB);    
    // env.cpu.ld_r8_r8(0b000, 0b001);
    // uint8_t r8_l = env.cpu.get_r8(0b000);
    // uint8_t r8_r = env.cpu.get_r8(0b001);
    // assert(r8_l == r8_r);

    for(int i = 0; i <= 7; ++i){
	env.cpu.set_r8(i, 0x00);
	for(int j = 0; j <= 7; ++j){
	    uint8_t value {};
	    if(j!=6){
		if(i!=6){
		    env.cpu.set_r8(j, 0xAA);
		    value = env.cpu.get_r8(j);
		    env.cpu.ld_r8_r8(i, j);
		    assert(value == env.cpu.get_r8(i));
		}
	    }
	}
    }

    //-----[ (HL)memory R/W test ld_r8_r8() ]-----------------------------------

    env.bus.write(0xC000, 0xAA);
    env.cpu.setHL(0xC000);
    for(int reg_code_l = 0; reg_code_l <=7; ++reg_code_l){
	if(reg_code_l != 4 && reg_code_l != 5){
	    uint8_t reg_code_r = 6;
	    if(reg_code_l == 6){
	        continue;
	    }	    
	    env.cpu.ld_r8_r8(reg_code_l, reg_code_r);
	    assert(env.cpu.get_r8(reg_code_l) == env.cpu.get_r8(reg_code_r));
	}
    }

    //-----[ L->(HL); H->(HL); (HL)->L; (HL)->H ld_r8_r8() ]--------------------

    // L->(HL)
    env.bus.write(0xC000, 0xBB);
    env.cpu.setHL(0xC000);

    uint8_t H_reg_code  = 0b100;
    uint8_t L_reg_code  = 0b101;
    uint8_t HL_reg_code = 0b110;
    
    env.cpu.ld_r8_r8(HL_reg_code, L_reg_code);
    assert(env.cpu.get_r8(HL_reg_code) == env.cpu.get_r8(L_reg_code));

    // H->(HL)
    env.bus.write(0xC000, 0xBB);
    env.cpu.ld_r8_r8(HL_reg_code, H_reg_code);
    assert(env.cpu.get_r8(HL_reg_code) == env.cpu.get_r8(H_reg_code));

    // (HL)->L
    env.bus.write(0xC000, 0xBB);
    uint8_t temp_HL = env.cpu.get_r8(HL_reg_code);
    env.cpu.ld_r8_r8(L_reg_code, HL_reg_code);
    assert(env.cpu.get_r8(L_reg_code) == temp_HL);

    // (HL)->H
    env.bus.write(0xC000, 0xB2);
    env.cpu.setHL(0xC000);
    temp_HL = env.cpu.get_r8(HL_reg_code);
    env.cpu.ld_r8_r8(H_reg_code, HL_reg_code);
    assert(env.cpu.get_r8(H_reg_code) == temp_HL);

    //-----[ ld_r8_n8() ]-------------------------------------------------------
    
    env.cpu.setPC(0xC000);
    env.bus.write(0xC001, 0xAA);    
    env.cpu.ld_r8_n8(0b000);
    uint8_t r8_l = env.cpu.get_r8(0b000);
    assert(r8_l == 0xAA);

    //-----[ ld_r16_n16() ]-----------------------------------------------------
    
    env.cpu.setPC(0xC000);
    env.bus.write(0xC001, 0xAA);
    env.bus.write(0xC002, 0xBB);    

    uint16_t r16 {};
    
    for(int i = 0; i <= 3; ++i){
	env.cpu.ld_r16_n16(i);
	r16 = env.cpu.get_r16rp(i);	
	assert(r16 == 0xBBAA);
	env.cpu.setPC(0xC000);
    }
    
    //--------------------------------------------------------------------------
}

//------------------------------------------------------------------------------

void test_cpu_decode()
{
    TestEnv env;

    // ld_r8_r8(dest_reg_code, src_reg_code)
    env.cpu.set_r8(0b000, 0xAA);
    env.cpu.set_r8(0b001, 0xBB);
    // 0b[0][0][d][s][t][s][r][c]; C->B
    env.bus.write(0xC000, 0b01000001);
    env.cpu.setPC(0xC000);
    env.cpu.decode();

    assert(env.cpu.get_r8(0b000) == 0xBB);
}

//------------------------------------------------------------------------------

int run_cpu_tests()
{
    test_cpu_register_pairs();
    test_cpu_opcode_field_decoding();
    test_cpu_get_r8_generic_access();
    test_cpu_fetch();
    test_cpu_decode();
    test_cpu_get_n8n16();
    test_cpu_get_r16mem();
    test_cpu_get_r16rp();
    test_cpu_set_r16rp();
    test_cpu_get_r16rp2();
    test_cpu_set_r16rp2();
    test_cpu_instructions_load();

    return 0;
}
