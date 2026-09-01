#include "../cpu.h"
#include <cassert>

namespace {
struct TestEnv {
    Sram sram;
    Cartridge cartridge;
    Bus bus;
    CPU cpu;

    TestEnv()
        : cartridge(sram, std::vector<uint8_t>(0x8000, 0x00)), bus(cartridge),
          cpu(bus) {}
};

//--------------------------------------------------------------------------

constexpr uint8_t OPCODE_CYCLES[256] = {
    // 0x00 - 0x0F
    4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4,
    // 0x10 - 0x1F
    4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,
    // 0x20 - 0x2F
    12, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,
    // 0x30 - 0x3F
    12, 12, 8, 8, 12, 12, 10, 4, 12, 8, 8, 8, 4, 4, 8, 4,
    // 0x40 - 0x4F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0x50 - 0x5F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0x60 - 0x6F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0x70 - 0x7F
    8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0x80 - 0x8F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0x90 - 0x9F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0xA0 - 0xAF
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0xB0 - 0xBF
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
    // 0xC0 - 0xCF
    20, 12, 16, 16, 24, 16, 8, 16, 20, 16, 16, 4, 24, 24, 8, 16,
    // 0xD0 - 0xDF
    20, 12, 16, 0, 24, 16, 8, 16, 20, 16, 16, 0, 24, 0, 8, 16,
    // 0xE0 - 0xEF
    12, 12, 8, 0, 0, 16, 8, 16, 16, 4, 16, 0, 0, 0, 8, 16,
    // 0xF0 - 0xFF
    12, 12, 8, 4, 0, 16, 8, 16, 12, 8, 16, 4, 0, 0, 8, 16};

//--------------------------------------------------------------------------

void poison_state(TestEnv &env) {
    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {
        env.cpu.set_r8(reg_code, 0xFF);
    }
}

//--------------------------------------------------------------------------

void expect_eq(uint8_t actual, uint8_t expected, const char *context) {
    if (actual != expected) {
        std::printf("FAIL: %s|expected=0x%02X actual=0x%02X\n", context,
                    expected, actual);
        std::fflush(stdout);
        std::abort();
    }
}

//--------------------------------------------------------------------------

void expect_eq(uint32_t actual, uint32_t expected, const char *context) {
    if (actual != expected) {
        std::printf("FAIL: %s | expected=%u actual=%u\n", context, expected,
                    actual);
        std::fflush(stdout);
        std::abort();
    }
}

//--------------------------------------------------------------------------

void poison_flag(TestEnv &env) { env.cpu.setF(0xFF); }

} // namespace

//------------------------------------------------------------------------------

void test_cpu_register_pairs() {
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

void assert_region_readable(TestEnv &env, uint16_t start, uint16_t end,
                            uint8_t value) {
    for (uint16_t address = start; address <= end; address += 0x1) {
        uint32_t t_cycles_before = env.cpu.cycles();
        env.bus.write(address, value);
        uint8_t result = env.cpu.read_byte(address);
        assert(result == value);
        assert(env.cpu.cycles() == t_cycles_before + 4);
    }
}

//------------------------------------------------------------------------------

void test_cpu_byte_read() {
    TestEnv env;

    assert_region_readable(env, 0xC000, 0xDFFF, 0xAB);
    assert_region_readable(env, 0x8000, 0x9FFF, 0xBC);
    assert_region_readable(env, 0xFF80, 0xFFFE, 0xCA);
}

//------------------------------------------------------------------------------

void assert_region_writable(TestEnv &env, uint16_t start, uint16_t end,
                            uint8_t value) {
    for (uint16_t address = start; address <= end; address += 0x1) {
        uint32_t t_cycles_before = env.cpu.cycles();
        env.cpu.write_byte(address, value);
        uint8_t result = env.bus.read(address);
        assert(result == value);
        assert(env.cpu.cycles() == t_cycles_before + 4);
    }
}

//------------------------------------------------------------------------------

void test_cpu_byte_write() {
    TestEnv env;

    assert_region_writable(env, 0xC000, 0xCFFF, 0xAB);
    assert_region_writable(env, 0x8000, 0x9FFF, 0xBC);
    assert_region_writable(env, 0xFF80, 0xFFFE, 0xCA);
}

//------------------------------------------------------------------------------

void test_cpu_opcode_field_decoding() {
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

void test_cpu_get_r8_generic_access() {
    TestEnv env;

    // set_r8(), get_r8()
    for (uint8_t i = 0; i <= 7; i++) {
        if (i == 6)
            continue;
        env.cpu.set_r8(i, 0xAA);
        // 6(0b110) [HL] pair register must be written to memory, not to
        // register
    }

    env.cpu.setHL(0xC000);
    env.cpu.set_r8(0b110, 0xAA);
    assert(env.bus.read(0xC000) == 0xAA);
}

//------------------------------------------------------------------------------

void test_cpu_get_n8n16() {
    TestEnv env;

    env.cpu.setPC(0xC000);
    env.bus.write(0xC000, 0xAA);
    uint8_t byte = env.cpu.get_n8();
    uint16_t current_pc = env.cpu.getPC();

    assert(byte == 0xAA);
    assert(current_pc == 0xC001);

    env.cpu.setPC(0xC000);
    env.bus.write(0xC000, 0xAA);
    env.bus.write(0xC001, 0xBB);
    uint16_t bytes = env.cpu.get_n16();
    current_pc = env.cpu.getPC();

    assert(bytes == 0xBBAA);
    assert(current_pc == 0xC002);
}

//------------------------------------------------------------------------------

void test_cpu_fetch() {
    TestEnv env;

    uint32_t t_cycles_before = env.cpu.cycles();

    env.bus.write(0x0000, 0x0A);
    env.bus.write(0x8000, 0xAB);
    env.cpu.setPC(0x8000);

    uint8_t byte = env.cpu.fetch();

    assert(byte == 0xAB);
    assert(env.cpu.getPC() == 0x8001);
    assert(env.cpu.cycles() == t_cycles_before + 4);
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp() {
    TestEnv env;

    uint16_t bytes{};

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

void test_cpu_set_r16rp() {
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

void test_cpu_get_r16rp2() {
    TestEnv env;

    uint16_t bytes{};

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

void test_cpu_set_r16rp2() {
    TestEnv env;

    env.cpu.set_r16rp2(0b00, 0x4243);
    assert(0x4243 == env.cpu.getBC());

    env.cpu.set_r16rp2(0b01, 0x4445);
    assert(0x4445 == env.cpu.getDE());

    env.cpu.set_r16rp2(0b10, 0x484C);
    assert(0x484C == env.cpu.getHL());

    env.cpu.set_r16rp2(0b11, 0x4146);
    assert(0x4140 == env.cpu.getAF());
}

//------------------------------------------------------------------------------

void test_cpu_get_r16mem() {
    TestEnv env;

    uint8_t byte{};

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

void test_cpu_instructions_load() {
    TestEnv env;

    //-----[ regular ld_r8_r8() ]-----------------------------------------------

    // env.cpu.setBC(0xAABB);
    // env.cpu.ld_r8_r8(0b000, 0b001);
    // uint8_t r8_l = env.cpu.get_r8(0b000);
    // uint8_t r8_r = env.cpu.get_r8(0b001);
    // assert(r8_l == r8_r);

    for (uint8_t i = 0; i <= 7; ++i) {
        env.cpu.set_r8(i, 0x00);
        for (uint8_t j = 0; j <= 7; ++j) {
            uint8_t value{};
            if (j != 6) {
                if (i != 6) {
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
    for (uint8_t reg_code_l = 0; reg_code_l <= 7; ++reg_code_l) {
        if (reg_code_l != 4 && reg_code_l != 5) {
            uint8_t reg_code_r = 6;
            if (reg_code_l == 6) {
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

    uint8_t H_reg_code = 0b100;
    uint8_t L_reg_code = 0b101;
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

    // env.cpu.setPC(0xC000);
    // env.bus.write(0xC000, 0xAA);
    // uint8_t byte = env.cpu.get_n8();

    // assert(byte == 0xAA);

    for (uint8_t opcode = 0x06; opcode <= 0x36; opcode += 0x10) {
        if (opcode == 0x36) {
            env.cpu.setHL(0xC000);
        }
        env.cpu.setPC(0xC000);
        uint8_t value = 0x00 + opcode;
        env.bus.write(0xC000, value);
        uint8_t reg_code = env.cpu.decode_r8_dest(opcode);
        env.cpu.ld_r8_n8(reg_code);
        uint8_t r8 = env.cpu.get_r8(reg_code);
        assert(r8 == value);
    }

    //-----[ ld_r16_n16() ]-----------------------------------------------------

    // env.cpu.setPC(0xC000);
    // env.bus.write(0xC001, 0xAA);
    // env.bus.write(0xC002, 0xBB);

    // uint16_t r16 {};

    for (uint8_t opcode = 0x01; opcode <= 0x31; opcode += 0x10) {
        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint16_t value = 0x1234 + opcode;
        uint8_t low_byte = static_cast<uint8_t>(value);
        uint8_t high_byte = static_cast<uint8_t>(value >> 8);
        env.bus.write(test_pc, low_byte);
        env.bus.write(test_pc + 1, high_byte);
        uint8_t reg_code = env.cpu.decode_r16_dest(opcode);
        env.cpu.ld_r16_n16(reg_code);
        uint16_t r16 = env.cpu.get_r16rp(reg_code);
        assert(r16 == value);
        assert(env.cpu.getPC() == test_pc + 2);
    }

    //--------------------------------------------------------------------------
}

//------------------------------------------------------------------------------

void test_cpu_decode() {
    TestEnv env;

    //-----[ ld_r8_r8(dest_reg_code, src_reg_code) ]----------------------------

    for (uint8_t dst = 0b000; dst <= 0b111; dst++) {
        for (uint8_t src = 0b000; src <= 0b111; src++) {
            if (!(dst == 0b110 && src == 0b110)) { // if not LD [HL], [HL]
                char ctx[64];
                std::snprintf(ctx, sizeof(ctx), "dst=%u src=%u", dst, src);

                env.cpu.setPC(0xC000);

                uint16_t test_pc = env.cpu.getPC();
                uint8_t test_opcode = ((0b01 << 6) | (dst << 3) | src);
                uint8_t test_value = dst + src + 0x10;
                env.bus.write(test_pc, test_opcode);

                poison_state(env);
                poison_flag(env);

                if (src == 0b110) {
                    env.bus.write(0xC500, test_value);
                    env.cpu.setHL(0xC500);
                    uint8_t expected = test_value;
                    uint8_t expected_f = env.cpu.getF();
                    uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];
                    uint32_t cycles_before = env.cpu.cycles();
                    env.cpu.decode();
                    uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
                    uint8_t actual = env.cpu.get_r8(dst);
                    uint8_t actual_f = env.cpu.getF();
                    expect_eq(actual, expected, ctx);
                    expect_eq(actual_f, expected_f, ctx);
                    expect_eq(actual_t_cycles, expected_t_cycles, ctx);
                } else if (dst == 0b110) {
                    if (src == 0b100) { // LD [HL], H
                        env.cpu.set_r8(src, test_value);
                        env.cpu.setHL(0xC500);
                        uint8_t expected = 0xC5;
                        uint8_t expected_f = env.cpu.getF();
                        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];
                        uint32_t cycles_before = env.cpu.cycles();
                        env.cpu.decode();
                        uint32_t actual_t_cycles =
                            env.cpu.cycles() - cycles_before;
                        uint8_t actual = env.cpu.get_r8(dst);
                        uint8_t actual_f = env.cpu.getF();
                        expect_eq(actual, expected, ctx);
                        expect_eq(actual_f, expected_f, ctx);
                        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
                    } else if (src == 0b101) { // LD [HL], L
                        env.cpu.setHL(0xC500);
                        uint8_t expected = 0x00;
                        uint8_t expected_f = env.cpu.getF();
                        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];
                        uint32_t cycles_before = env.cpu.cycles();
                        env.cpu.decode();
                        uint32_t actual_t_cycles =
                            env.cpu.cycles() - cycles_before;
                        uint8_t actual = env.cpu.get_r8(dst);
                        uint8_t actual_f = env.cpu.getF();
                        expect_eq(actual, expected, ctx);
                        expect_eq(actual_f, expected_f, ctx);
                        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
                    } else { // LD [HL], 8
                        env.cpu.setHL(0xC500);
                        env.cpu.set_r8(src, test_value);
                        uint8_t expected = test_value;
                        uint8_t expected_f = env.cpu.getF();
                        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];
                        uint32_t cycles_before = env.cpu.cycles();
                        env.cpu.decode();
                        uint32_t actual_t_cycles =
                            env.cpu.cycles() - cycles_before;
                        uint8_t actual = env.cpu.get_r8(dst);
                        uint8_t actual_f = env.cpu.getF();
                        expect_eq(actual, expected, ctx);
                        expect_eq(actual_f, expected_f, ctx);
                        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
                    }
                } else {
                    env.cpu.set_r8(src, test_value);
                    uint8_t expected = test_value;
                    uint8_t expected_f = env.cpu.getF();
                    uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];
                    uint32_t cycles_before = env.cpu.cycles();
                    env.cpu.decode();
                    uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
                    uint8_t actual = env.cpu.get_r8(dst);
                    uint8_t actual_f = env.cpu.getF();
                    expect_eq(actual, expected, ctx);
                    expect_eq(actual_f, expected_f, ctx);
                    expect_eq(actual_t_cycles, expected_t_cycles, ctx);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

int run_cpu_tests() {
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
    test_cpu_byte_read();
    test_cpu_byte_write();

    return 0;
}
