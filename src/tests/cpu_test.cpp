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

//------------------------------------------------------------------------------

constexpr uint8_t OPCODE_CYCLES[256] = {
    // 0x00 - 0x0F
    4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4,
    // 0x10 - 0x1F
    4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,
    // 0x20 - 0x2F
    12, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,
    // 0x30 - 0x3F
    12, 12, 8, 8, 12, 12, 12, 4, 12, 8, 8, 8, 4, 4, 8, 4,
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

//------------------------------------------------------------------------------

void poison_state(TestEnv &env) {
    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {
        env.cpu.set_r8(reg_code, 0xFF);
    }
}

//------------------------------------------------------------------------------

void setup_known_registers(TestEnv &env) {
    env.cpu.setBC(0x1234);
    env.cpu.setDE(0x5678);
    env.cpu.setHL(0x9ABC);
    env.cpu.setAF(0xDEF0);
}

//------------------------------------------------------------------------------

void expect_eq(uint8_t actual, uint8_t expected, const char *context) {
    if (actual != expected) {
        std::printf("FAIL: %s | expected=0x%02X actual=0x%02X\n", context,
                    expected, actual);
        std::fflush(stdout);
        std::abort();
    }
}

//------------------------------------------------------------------------------

void expect_eq(uint16_t actual, uint16_t expected, const char *context) {
    if (actual != expected) {
        std::printf("FAIL: %s | expected=0x%02X actual=0x%02x\n", context,
                    expected, actual);
        std::fflush(stdout);
        std::abort();
    }
}

//------------------------------------------------------------------------------

void expect_eq(uint32_t actual, uint32_t expected, const char *context) {
    if (actual != expected) {
        std::printf("FAIL: %s | expected_t_cycles=%u actual_t_cycles=%u\n",
                    context, expected, actual);
        std::fflush(stdout);
        std::abort();
    }
}

//------------------------------------------------------------------------------

void expect_eq(bool actual, bool expected, const char *context) {
    if (actual != expected) {
        std::printf("FAIL: %s | expected=%s actual=%s\n", context,
                    expected ? "true" : "false", actual ? "true" : "false");
        std::fflush(stdout);
        std::abort();
    }
}

//------------------------------------------------------------------------------

void poison_flag(TestEnv &env) { env.cpu.setF(false, false, false, false); }

} // namespace

//------------------------------------------------------------------------------

void test_cpu_register_pairs() {

    TestEnv env;

    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_register_pairs(): BC pair");

        poison_state(env);

        uint16_t test_value = 0x1234;

        uint8_t expected_high_byte = static_cast<uint8_t>(test_value >> 8);
        uint8_t expected_low_byte = static_cast<uint8_t>(test_value);

        env.cpu.setBC(test_value);

        uint8_t actual_high_byte = env.cpu.get_r8(0b000);
        uint8_t actual_low_byte = env.cpu.get_r8(0b001);

        expect_eq(actual_high_byte, expected_high_byte, ctx);
        expect_eq(actual_low_byte, expected_low_byte, ctx);
    }

    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_register_pairs(): DE pair");

        poison_state(env);

        uint16_t test_value = 0x1234;

        uint8_t expected_high_byte = static_cast<uint8_t>(test_value >> 8);
        uint8_t expected_low_byte = static_cast<uint8_t>(test_value);

        env.cpu.setDE(test_value);

        uint8_t actual_high_byte = env.cpu.get_r8(0b010);
        uint8_t actual_low_byte = env.cpu.get_r8(0b011);

        expect_eq(actual_high_byte, expected_high_byte, ctx);
        expect_eq(actual_low_byte, expected_low_byte, ctx);
    }

    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_register_pairs(): HL pair");

        poison_state(env);

        uint16_t test_value = 0x1234;

        uint8_t expected_high_byte = static_cast<uint8_t>(test_value >> 8);
        uint8_t expected_low_byte = static_cast<uint8_t>(test_value);

        env.cpu.setHL(test_value);

        uint8_t actual_high_byte = env.cpu.get_r8(0b100);
        uint8_t actual_low_byte = env.cpu.get_r8(0b101);

        expect_eq(actual_high_byte, expected_high_byte, ctx);
        expect_eq(actual_low_byte, expected_low_byte, ctx);
    }

    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_register_pairs(): AF");

        poison_state(env);

        uint16_t test_value = 0x1234;

        uint8_t expected_high_byte = static_cast<uint8_t>(test_value >> 8);
        uint8_t expected_low_byte = static_cast<uint8_t>(test_value & 0xF0);

        env.cpu.setAF(test_value);

        uint8_t actual_high_byte = env.cpu.get_r8(0b111);
        uint8_t actual_low_byte = env.cpu.getF();

        expect_eq(actual_high_byte, expected_high_byte, ctx);
        expect_eq(actual_low_byte, expected_low_byte, ctx);
    }

    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_register_pairs(): SP");

        poison_state(env);

        uint16_t test_value = 0xAABB;
        uint16_t expected = test_value;

        env.cpu.setSP(test_value);

        uint16_t actual = env.cpu.getSP();

        expect_eq(actual, expected, ctx);
    }

    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_register_pairs(): PC");

        uint16_t test_value = 0xBBAA;
        uint16_t expected = test_value;

        env.cpu.setPC(test_value);

        uint16_t actual = env.cpu.getPC();

        expect_eq(actual, expected, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_get_flag_z() {

    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0x7F);

        bool expected = false;

        bool actual = env.cpu.get_flag_z();

        expect_eq(actual, expected, "test_cpu_get_flag_z(): test_value=0x7F");
    }

    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0x80);

        bool expected = true;

        bool actual = env.cpu.get_flag_z();

        expect_eq(actual, expected, "test_cpu_get_flag_z(): test_value=0x80");
    }
}

//------------------------------------------------------------------------------

void test_cpu_get_flag_n() {

    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0xBF);

        bool expected = false;

        bool actual = env.cpu.get_flag_n();

        expect_eq(actual, expected, "test_cpu_get_flag_n(): test_value=0xBF");
    }

    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0x40);

        bool expected = true;

        bool actual = env.cpu.get_flag_n();

        expect_eq(actual, expected, "test_cpu_get_flag_n(): test_value=0x40");
    }
}

//------------------------------------------------------------------------------

void test_cpu_get_flag_h() {
    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0xDF);

        bool expected = false;

        bool actual = env.cpu.get_flag_h();

        expect_eq(actual, expected, "test_cpu_get_flag_h(): test_value=0xDF");
    }

    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0x20);

        bool expected = true;

        bool actual = env.cpu.get_flag_h();

        expect_eq(actual, expected, "test_cpu_get_flag_h(): test_value=0x20");
    }
}

//------------------------------------------------------------------------------

void test_cpu_get_flag_c() {
    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0xEF);

        bool expected = false;

        bool actual = env.cpu.get_flag_c();

        expect_eq(actual, expected, "test_cpu_get_flag_c(): test_value=0xEF");
    }

    {
        TestEnv env;

        poison_flag(env);

        env.cpu.setF(0x10);

        bool expected = true;

        bool actual = env.cpu.get_flag_c();

        expect_eq(actual, expected, "test_cpu_get_flag_c(): test_value=0x10");
    }
}

//------------------------------------------------------------------------------

void test_cpu_flag_helppers() {}

//------------------------------------------------------------------------------

void test_cpu_byte_read() {

    TestEnv env;

    char ctx[64];
    std::snprintf(ctx, sizeof(ctx), "test_cpu_byte_read()");

    uint8_t test_value = 0xAA;
    uint16_t test_address = 0xC000;

    env.bus.write(test_address, test_value);

    uint8_t expected = test_value;
    uint32_t expected_t_cycles = 4;

    uint8_t actual = env.cpu.read_byte(test_address);
    uint32_t actual_t_cycles = env.cpu.cycles();

    expect_eq(actual, expected, ctx);
    expect_eq(actual_t_cycles, expected_t_cycles, ctx);
}

//------------------------------------------------------------------------------

void test_cpu_byte_write() {

    TestEnv env;

    char ctx[64];
    std::snprintf(ctx, sizeof(ctx), "test_cpu_byte_write()");

    uint8_t test_value = 0xDD;
    uint32_t test_address = 0xC000;

    uint8_t expected = test_value;
    uint32_t expected_t_cycles = 4;

    env.cpu.write_byte(test_address, test_value);

    uint8_t actual = env.bus.read(test_address);
    uint32_t actual_t_cycles = env.cpu.cycles();

    expect_eq(actual, expected, ctx);
    expect_eq(actual_t_cycles, expected_t_cycles, ctx);
}

//------------------------------------------------------------------------------

void test_cpu_opcode_field_decoding() {
    // get_r8_dest, get_r8_source, get_r16_dest

    TestEnv env;

    char ctx[64];

    uint8_t expected{};
    uint8_t actual{};

    {
        expected = 0b101;
        actual = env.cpu.decode_r8_dest(0x6C);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_opcode_field_decoding(): r8_dest(0x6C)");

        expect_eq(actual, expected, ctx);
    }

    {
        expected = 0b100;
        actual = env.cpu.decode_r8_source(0x6C);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_opcode_field_decoding(): r8_source(0x6C)");

        expect_eq(actual, expected, ctx);
    }

    {
        expected = 0b10;
        actual = env.cpu.decode_r16_dest(0x6C);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_opcode_field_decoding(): r16_dest(0x6C)");

        expect_eq(actual, expected, ctx);
    }

    //--------------------------------------------------------------------------

    {
        expected = 0b011;
        actual = env.cpu.decode_r8_dest(0x1B);
        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_opcode_field_decoding(): r8_dest(0x1B)");

        expect_eq(actual, expected, ctx);
    }

    {
        expected = 0b011;
        actual = env.cpu.decode_r8_source(0x1B);
        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_opcode_field_decoding(): r8_source(0x1B)");

        expect_eq(actual, expected, ctx);
    }

    {
        expected = 0b01;
        actual = env.cpu.decode_r16_dest(0x1B);
        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_opcode_field_decoding(): r16_source(0x1B)");

        expect_eq(actual, expected, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_set_r8() {

    TestEnv env;

    setup_known_registers(env);
    env.cpu.set_r8(0b000, 0x99);
    expect_eq(env.cpu.getBC(), 0x9934, "test_cpu_set_r8(B, 0x99)");

    setup_known_registers(env);
    env.cpu.set_r8(0b001, 0x99);
    expect_eq(env.cpu.getBC(), 0x1299, "test_cpu_set_r8(C, 0x99)");

    setup_known_registers(env);
    env.cpu.set_r8(0b010, 0x99);
    expect_eq(env.cpu.getDE(), 0x9978, "test_cpu_set_r8(D, 0x99)");

    setup_known_registers(env);
    env.cpu.set_r8(0b011, 0x99);
    expect_eq(env.cpu.getDE(), 0x5699, "test_cpu_set_r8(E, 0x99)");

    setup_known_registers(env);
    env.cpu.set_r8(0b100, 0x99);
    expect_eq(env.cpu.getHL(), 0x99BC, "test_cpu_set_r8(H, 0x99)");

    setup_known_registers(env);
    env.cpu.set_r8(0b101, 0x99);
    expect_eq(env.cpu.getHL(), 0x9A99, "test_cpu_set_r8(L, 0x99)");

    setup_known_registers(env);
    env.cpu.set_r8(0b111, 0x99);
    expect_eq(env.cpu.getAF(), 0x99F0, "test_cpu_set_r8(A, 0x99)");
}

//------------------------------------------------------------------------------

void test_cpu_set_r8_hl_indirect() {

    TestEnv env;

    setup_known_registers(env);
    uint16_t hl_before = env.cpu.getHL();
    env.cpu.set_r8(0b110, 0x99);
    expect_eq(env.bus.read(0x9ABC), 0x99, "test_cpu_set_r8_hl_indirect()");
    expect_eq(env.cpu.getHL(), hl_before,
              "test_cpu_set_r8_hl_indirect(): modified HL!");
}

//------------------------------------------------------------------------------

void test_cpu_get_n8() {

    TestEnv env;

    env.cpu.setPC(0xC000);
    uint8_t test_value = 0x6C;
    uint16_t test_pc = env.cpu.getPC();
    env.bus.write(test_pc, test_value);

    uint8_t expected = test_value;
    uint16_t expected_pc = test_pc + 1;

    uint8_t actual = env.cpu.get_n8();
    uint16_t actual_pc = env.cpu.getPC();

    expect_eq(actual, expected, "test_cpu_get_n8(): value");
    expect_eq(actual_pc, expected_pc, "test_cpu_get_n8(): pc");
}

//------------------------------------------------------------------------------

void test_cpu_get_n16() {

    TestEnv env;

    uint8_t high_byte = 0x12;
    uint8_t low_byte = 0x34;

    env.cpu.setPC(0xC000);

    uint16_t test_pc = env.cpu.getPC();
    uint16_t test_value = (high_byte << 8) | low_byte;

    env.bus.write(test_pc + 1, high_byte);
    env.bus.write(test_pc, low_byte);

    uint16_t expected = test_value;
    uint16_t expected_pc = test_pc + 2;

    uint16_t actual = env.cpu.get_n16();
    uint16_t actual_pc = env.cpu.getPC();

    expect_eq(actual, expected, "test_cpu_get_n16(): value");
    expect_eq(actual_pc, expected_pc, "test_cpu_get_n16(): pc");
}

//------------------------------------------------------------------------------

void test_cpu_fetch_basic() {

    TestEnv env;

    char ctx[64];
    std::snprintf(ctx, sizeof(ctx), "test_cpu_fetch_basic(): ");

    env.cpu.setPC(0xC000);
    uint16_t test_pc = env.cpu.getPC();
    uint8_t test_value = 0x6C;
    env.bus.write(test_pc, test_value);

    uint8_t expected = test_value;
    uint16_t expected_pc = test_pc + 1;

    uint8_t actual = env.cpu.fetch();
    uint16_t actual_pc = env.cpu.getPC();

    // uint8_t actual = env.bus.read(test_pc + 1);

    expect_eq(actual, expected, ctx);
    expect_eq(actual_pc, expected_pc, "test_cpu_fetch_basic(): pc, ");
}

//------------------------------------------------------------------------------

void test_cpu_fetch_wraparound() {

    TestEnv env;

    env.cpu.setPC(0xFFFF);
    uint16_t test_pc = env.cpu.getPC();
    uint8_t test_value = 0x6C;
    env.bus.write(test_pc, test_value);

    uint8_t expected = test_value;
    uint16_t expected_pc = 0x0000;

    uint8_t actual = env.cpu.fetch();
    uint16_t actual_pc = env.cpu.getPC();

    expect_eq(actual, expected, "test_cpu_fetch_wraparound(): value, ");
    expect_eq(actual_pc, expected_pc,
              "test_cpu_fetch_wraparound(): wraparound, ");
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp() {

    TestEnv env;

    uint16_t bytes{};

    setup_known_registers(env);

    env.cpu.setBC(0x4243);
    bytes = env.cpu.get_r16rp(0b00);
    expect_eq(bytes, 0x4243, "test_cpu_get_r16rp(): BC");

    setup_known_registers(env);

    env.cpu.setDE(0x4445);
    bytes = env.cpu.get_r16rp(0b01);
    expect_eq(bytes, 0x4445, "test_cpu_get_r16rp(): DE");

    setup_known_registers(env);

    env.cpu.setHL(0x484C);
    bytes = env.cpu.get_r16rp(0b10);
    expect_eq(bytes, 0x484C, "test_cpu_get_r16rp(): HL");

    setup_known_registers(env);

    env.cpu.setSP(0x5350);
    bytes = env.cpu.get_r16rp(0b11);
    expect_eq(bytes, 0x5350, "test_cpu_get_r16rp(): SP");
}

//------------------------------------------------------------------------------

void test_cpu_set_r16rp() {

    TestEnv env;

    uint16_t bytes{};

    setup_known_registers(env);

    env.cpu.set_r16rp(0b00, 0x4243);
    bytes = env.cpu.getBC();
    expect_eq(bytes, 0x4243, "test_cpu_set_r16rp(): bc");

    setup_known_registers(env);

    env.cpu.set_r16rp(0b01, 0x4445);
    bytes = env.cpu.getDE();
    expect_eq(bytes, 0x4445, "test_cpu_set_r16rp(): de");

    setup_known_registers(env);

    env.cpu.set_r16rp(0b10, 0x484C);
    bytes = env.cpu.getHL();
    expect_eq(bytes, 0x484C, "test_cpu_set_r16rp(): hl");

    setup_known_registers(env);

    env.cpu.set_r16rp(0b11, 0x5350);
    bytes = env.cpu.getSP();
    expect_eq(bytes, 0x5350, "test_cpu_set_r16rp(): sp");
}

//------------------------------------------------------------------------------

void test_cpu_get_r16rp2() {

    TestEnv env;

    uint16_t bytes{};

    setup_known_registers(env);

    env.cpu.setBC(0x4243);
    bytes = env.cpu.get_r16rp2(0b00);
    expect_eq(bytes, 0x4243, "test_cpu_get_r16rp2(): bc");

    setup_known_registers(env);

    env.cpu.setDE(0x4445);
    bytes = env.cpu.get_r16rp2(0b01);
    expect_eq(bytes, 0x4445, "test_cpu_get_r16rp2(): de");

    setup_known_registers(env);

    env.cpu.setHL(0x484C);
    bytes = env.cpu.get_r16rp2(0b10);
    expect_eq(bytes, 0x484C, "test_cpu_get_r16rp2(): hl");

    setup_known_registers(env);

    env.cpu.setAF(0x4146);
    bytes = env.cpu.get_r16rp2(0b11);
    expect_eq(bytes, 0x4140, "test_cpu_get_r16rp2(): af");
}

//------------------------------------------------------------------------------

void test_cpu_set_r16rp2() {

    TestEnv env;

    uint16_t bytes{};

    setup_known_registers(env);

    env.cpu.set_r16rp2(0b00, 0x4243);
    bytes = env.cpu.getBC();
    expect_eq(bytes, 0x4243, "test_cpu_set_r16rp2(): bc");

    env.cpu.set_r16rp2(0b01, 0x4445);
    bytes = env.cpu.getDE();
    expect_eq(bytes, 0x4445, "test_cpu_set_r16rp2(): de");

    env.cpu.set_r16rp2(0b10, 0x484C);
    bytes = env.cpu.getHL();
    expect_eq(bytes, 0x484C, "test_cpu_set_r16rp2(): hl");

    env.cpu.set_r16rp2(0b11, 0x4146);
    bytes = env.cpu.getAF();
    expect_eq(bytes, 0x4140, "test_cpu_set_r16rp2(): af");
}

//------------------------------------------------------------------------------

void test_cpu_set_r16mem() {

    char ctx[64];

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setBC(0xC000);

        uint8_t test_reg_code = 0b00;
        uint8_t test_value = 0x12;

        uint8_t expected = test_value;

        env.cpu.set_r16mem(test_reg_code, test_value);

        uint8_t actual = env.cpu.get_r16mem(test_reg_code);

        expect_eq(actual, expected, "test_cpu_set_r16mem(): value -> bc");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setDE(0xC000);

        uint8_t test_reg_code = 0b01;
        uint8_t test_value = 0x23;

        uint8_t expected = test_value;

        env.cpu.set_r16mem(test_reg_code, test_value);

        uint8_t actual = env.cpu.get_r16mem(test_reg_code);

        expect_eq(actual, expected, "test_cpu_set_r16mem(): value -> de");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setHL(0xC000);

        uint8_t test_reg_code = 0b10;
        uint8_t test_value = 0x32;

        uint8_t expected = test_value;
        uint16_t expected_reg_val = env.cpu.getHL() + 1;

        env.cpu.set_r16mem(test_reg_code, test_value);

        uint8_t actual = env.bus.read(0xC000);
        uint16_t actual_reg_val = env.cpu.getHL();

        expect_eq(actual, expected, "test_cpu_set_r16mem(): value -> hl_inc");
        expect_eq(actual_reg_val, expected_reg_val,
                  "test_cpu_set_r16mem(): value -> hl_inc");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setHL(0xC000);

        uint8_t test_reg_code = 0b11;
        uint8_t test_value = 0x34;

        uint8_t expected = test_value;
        uint16_t expected_reg_val = env.cpu.getHL() - 1;

        env.cpu.set_r16mem(test_reg_code, test_value);

        uint8_t actual = env.bus.read(0xC000);
        uint16_t actual_reg_val = env.cpu.getHL();

        expect_eq(actual, expected, "test_cpu_set_r16mem(): value -> hl_dec");
        expect_eq(actual_reg_val, expected_reg_val,
                  "test_cpu_set_r16mem(): value -> hl_dec");
    }
}

//------------------------------------------------------------------------------

void test_cpu_get_r16mem() {

    TestEnv env;

    uint8_t byte{};

    char ctx[64];

    setup_known_registers(env);

    env.bus.write(0xC000, 0xAA);
    env.cpu.setBC(0xC000);
    byte = env.cpu.get_r16mem(0b00);

    expect_eq(byte, 0xAA, "test_cpu_get_r16mem(): bc");

    //--------------------------------------------------------------------------

    setup_known_registers(env);

    env.bus.write(0xC001, 0xBB);
    env.cpu.setDE(0xC001);
    byte = env.cpu.get_r16mem(0b01);

    expect_eq(byte, 0xBB, "test_cpu_get_r16mem(): de");

    //--------------------------------------------------------------------------

    {
        setup_known_registers(env);

        std::snprintf(ctx, sizeof(ctx), "test_cpu_get_r16mem():hl inc");

        env.bus.write(0xC002, 0xCC);
        env.cpu.setHL(0xC002);

        byte = env.cpu.get_r16mem(0b10);
        uint16_t expected_reg_val = 0xC003;
        uint16_t actual_reg_val = env.cpu.getHL();

        expect_eq(byte, 0xCC, "test_cpu_get_r16mem(): hl");
        expect_eq(actual_reg_val, expected_reg_val, ctx);
    }

    //--------------------------------------------------------------------------

    {
        setup_known_registers(env);

        std::snprintf(ctx, sizeof(ctx), "test_cpu_get_r16mem():hl dec");

        env.bus.write(0xC010, 0xDD);
        env.cpu.setHL(0xC010);

        byte = env.cpu.get_r16mem(0b11);
        uint16_t expected_reg_val = 0xC00F;
        uint16_t actual_reg_val = env.cpu.getHL();

        expect_eq(byte, 0xDD, "test_cpu_get_r16mem(): hl");
        expect_eq(actual_reg_val, expected_reg_val, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_ld_r16_n16() {

    TestEnv env;

    for (uint8_t reg_code = 0b00; reg_code <= 0b11; reg_code++) {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "reg_code=%u", reg_code);

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint16_t test_value = reg_code + 0x02;
        uint8_t high_byte = static_cast<uint8_t>(test_value >> 8);
        uint8_t low_byte = static_cast<uint8_t>(test_value & 0x00FF);
        env.bus.write(test_pc, low_byte);
        env.bus.write(test_pc + 1, high_byte);

        uint16_t expected = test_value;
        uint16_t expected_pc = test_pc + 2;
        uint32_t expected_t_cycles = 8; // 12(optable) - 4(fetch) = 8
        uint32_t cycles_before = env.cpu.cycles();

        env.cpu.ld_r16_n16(reg_code);

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint16_t actual = env.cpu.get_r16rp(reg_code);
        uint16_t actual_pc = env.cpu.getPC();

        expect_eq(actual, expected, ctx);
        expect_eq(actual_pc, expected_pc, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_ld_r16mem_a() {

    {
        TestEnv env;

        poison_state(env);

        uint8_t value = 0x12;
        env.cpu.set_r8(0b111, value);

        uint8_t reg_code = 0b00;
        uint16_t address = 0xC000;
        env.cpu.setBC(address);

        uint8_t expected = value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4; // 8(optable) - 4(fetch) = 4

        env.cpu.ld_r16mem_a(reg_code);

        uint8_t actual = env.bus.read(address);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_r16mem_a(): a->bc value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_r16mem_a(): a->bc t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        uint8_t value = 0x23;
        env.cpu.set_r8(0b111, value);

        uint8_t reg_code = 0b01;
        uint16_t address = 0xC000;
        env.cpu.setDE(address);

        uint8_t expected = value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4; // 8(optable) - 4(fetch) = 4

        env.cpu.ld_r16mem_a(reg_code);

        uint8_t actual = env.bus.read(address);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_r16mem_a(): a->de value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_r16mem_a(): a->de t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        uint8_t value = 0x23;
        env.cpu.set_r8(0b111, value);

        uint8_t reg_code = 0b10;
        uint16_t address = 0xC000;
        env.cpu.setHL(address);

        uint8_t expected = value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4; // 8(optable) - 4(fetch) = 4

        env.cpu.ld_r16mem_a(reg_code);

        uint8_t actual = env.bus.read(address);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_r16mem_a(): a->hl_inc value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_r16mem_a(): a->hl_inc t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        uint8_t value = 0x34;
        env.cpu.set_r8(0b111, value);

        uint8_t reg_code = 0b11;
        uint16_t address = 0xC000;
        env.cpu.setHL(address);

        uint8_t expected = value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4; // 8(optable) - 4(fetch) = 4

        env.cpu.ld_r16mem_a(reg_code);

        uint8_t actual = env.bus.read(address);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_r16mem_a(): a->hl_decc value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_r16mem_a(): a->hl_dec t_cycles");
    }
}

//------------------------------------------------------------------------------

void test_cpu_inc_r16() {

    for (uint8_t test_opcode = 0x03; test_opcode <= 0x33; test_opcode += 0x10) {

        TestEnv env;

        char ctx[64];

        poison_state(env);

        uint8_t test_reg_code = (test_opcode >> 4) & 0x03;
        uint16_t test_value = test_reg_code + 0x02;
        env.cpu.set_r16rp(test_reg_code, test_value);

        uint8_t expected_flags = 0x00;
        uint16_t expected = test_value + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode] - 4;

        env.cpu.inc_r16(test_reg_code);

        uint8_t actual_flags = env.cpu.getF();
        uint16_t actual = env.cpu.get_r16rp(test_reg_code);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_inc_r16(): reg_code=%u, value", test_reg_code);

        expect_eq(actual, expected, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
        expect_eq(actual_flags, expected_flags, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_inc_r16_wraparound() {

    TestEnv env;

    poison_state(env);

    env.cpu.setBC(0xFFFF);

    uint16_t expected = 0x0000;
    uint32_t cycles_before = env.cpu.cycles();
    uint32_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4

    env.cpu.inc_r16(0b00);

    uint16_t actual = env.cpu.getBC();
    uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

    expect_eq(actual, expected, "test_cpu_inc_r16_wraparound(): value");
    expect_eq(actual_t_cycles, expected_t_cycles,
              "test_cpu_inc_r16_wraparound(): t_cycles");
}

//------------------------------------------------------------------------------

void test_cpu_inc_r16_byte_carry() {

    TestEnv env;

    poison_state(env);

    env.cpu.setBC(0x00FF);

    uint16_t expected = 0x0100;
    uint32_t cycles_before = env.cpu.cycles();
    uint32_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4

    env.cpu.inc_r16(0b00);

    uint16_t actual = env.cpu.getBC();
    uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

    expect_eq(actual, expected, "test_cpu_inc_r16_bit_carry(): value");
    expect_eq(actual_t_cycles, expected_t_cycles,
              "test_cpu_inc_r16_bit_carry(): t_cycles");
}

//------------------------------------------------------------------------------

void test_cpu_ld_a_r16mem() {

    {
        TestEnv env;

        poison_state(env);

        uint8_t test_reg_code = 0b00;

        env.cpu.setBC(0xC000);

        uint16_t test_address = env.cpu.getBC();
        uint8_t test_value = 0x12;

        env.bus.write(test_address, test_value);

        uint8_t expected = test_value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4;

        env.cpu.ld_a_r16mem(test_reg_code);

        uint8_t actual = env.cpu.get_r8(0b111);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_a_r16mem(): bc->a value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_a_r16mem(): bc->a t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        uint8_t test_reg_code = 0b01;

        env.cpu.setDE(0xC000);

        uint16_t test_address = env.cpu.getDE();
        uint8_t test_value = 0x12;

        env.bus.write(test_address, test_value);

        uint8_t expected = test_value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4;

        env.cpu.ld_a_r16mem(test_reg_code);

        uint8_t actual = env.cpu.get_r8(0b111);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_a_r16mem(): de->a value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_a_r16mem(): de->a t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        uint8_t test_reg_code = 0b010;

        env.cpu.setHL(0xC000);

        uint16_t test_address = env.cpu.getHL();
        uint8_t test_value = 0x12;

        env.bus.write(test_address, test_value);

        uint8_t expected = test_value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4;

        env.cpu.ld_a_r16mem(test_reg_code);

        uint8_t actual = env.cpu.get_r8(0b111);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_a_r16mem(): hl_inc->a value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_a_r16mem(): hl_inc->a t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        uint8_t test_reg_code = 0b011;

        env.cpu.setHL(0xC000);

        uint16_t test_address = env.cpu.getHL();
        uint8_t test_value = 0x12;

        env.bus.write(test_address, test_value);

        uint8_t expected = test_value;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = 4;

        env.cpu.ld_a_r16mem(test_reg_code);

        uint8_t actual = env.cpu.get_r8(0b111);
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, "test_cpu_ld_a_r16mem(): hl_dec->a value");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_ld_a_r16mem(): hl_dec->a t_cycles");
    }
}

//------------------------------------------------------------------------------

void test_cpu_inc_r8() {

    {
        TestEnv env;

        uint8_t test_reg_code = 0b001;
        uint8_t test_value = 0x00;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_inc_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value + 1;
        uint8_t expected_flags = 0b00000000;

        env.cpu.inc_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_inc_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0x0F;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_inc_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value + 1;
        uint8_t expected_flags = 0b00100000;

        env.cpu.inc_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_inc_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0x7F;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_inc_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value + 1;
        uint8_t expected_flags = 0b00100000;

        env.cpu.inc_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_inc_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0xFF;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_inc_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value + 1;
        uint8_t expected_flags = 0b10100000;

        env.cpu.inc_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_inc_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_dec_r8() {

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0x02;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_dec_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value - 1;
        uint8_t expected_flags = 0b01000000;

        env.cpu.dec_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_dec_r8(): value");

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_dec_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0x01;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_dec_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value - 1;
        uint8_t expected_flags = 0b11000000;

        env.cpu.dec_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_dec_r8(): value");

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_dec_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0x10;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_dec_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value - 1;
        uint8_t expected_flags = 0b01100000;

        env.cpu.dec_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_dec_r8(): value");

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_dec_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }

    {
        TestEnv env;

        uint8_t test_reg_code = 0b000;
        uint8_t test_value = 0x00;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_dec_r8(): test_value=0x%02X",
                      test_value);

        poison_state(env);
        poison_flag(env);

        env.cpu.set_r8(test_reg_code, test_value);

        uint8_t expected = test_value - 1;
        uint8_t expected_flags = 0b01100000;

        env.cpu.dec_r8(test_reg_code);

        uint8_t actual = env.cpu.get_r8(test_reg_code);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_dec_r8(): value");

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_dec_r8(): test_value=0x%02X, flags",
                      test_value);

        expect_eq(actual_flags, expected_flags, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_ld_r8_n8() {

    TestEnv env;

    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_cpu_ld_r8_n8(): reg_code=%u",
                      reg_code);

        poison_state(env);

        env.cpu.setPC(0xC000);

        if (reg_code == 6)
            env.cpu.setHL(0xC500);

        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_value = reg_code + 0x02;
        env.bus.write(test_pc, test_value);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles;

        if (reg_code == 6) {
            expected_t_cycles = 8;
        } else {
            expected_t_cycles = 4;
        }

        env.cpu.ld_r8_n8(reg_code);

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint16_t actual_pc = env.cpu.getPC();
        uint8_t actual{};

        if (reg_code == 6) {
            actual = env.bus.read(0xC500);
        } else {
            actual = env.cpu.get_r8(reg_code);
        }

        expect_eq(actual, expected, ctx);
        expect_eq(actual_pc, expected_pc, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_add_HL_r16() {

    for (uint8_t test_opcode = 0x09; test_opcode <= 0x39; test_opcode += 0x10) {

        TestEnv env;

        poison_state(env);
        poison_flag(env);

        char ctx[64];

        uint16_t test_reg_code = (test_opcode >> 4) & 0x03;

        uint16_t test_value_HL = 0x1000;
        env.cpu.setHL(test_value_HL);
        uint16_t test_value_r16 = 0x0100;
        env.cpu.set_r16rp(test_reg_code, test_value_r16);

        uint16_t expected{};

        if (test_reg_code == 0x02) {
            expected = test_value_r16 * 2;
        } else {
            expected = test_value_HL + test_value_r16;
        }

        uint16_t cycles_before = env.cpu.cycles();
        uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4
        uint8_t expected_flags = 0b00000000;

        env.cpu.add_HL_r16(test_reg_code);

        uint16_t actual = env.cpu.getHL();
        uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint8_t actual_flags = env.cpu.getF();

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_add_hl_r16(): "
                      "reg_code=%u, value",
                      test_reg_code);

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_add_hl_r16(): "
                      "reg_code=%u, t_cycles",
                      test_reg_code);

        expect_eq(actual_t_cycles, expected_t_cycles, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_add_hl_r16(): "
                      "reg_code=%u, flags",
                      test_reg_code);

        expect_eq(actual_flags, expected_flags, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_add_HL_r16_wraparound() {

    TestEnv env;

    poison_state(env);
    poison_flag(env);

    char ctx[64];

    uint16_t test_value_HL = 0xFFFF;

    env.cpu.setHL(test_value_HL);

    uint8_t test_reg_code = 0b00;
    uint16_t test_value_r16 = 0x1;

    env.cpu.set_r16rp(test_reg_code, test_value_r16);

    uint16_t expected = test_value_HL + test_value_r16;
    uint16_t cycles_before = env.cpu.cycles();
    uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4
    uint8_t expected_flags = 0b00110000;

    env.cpu.add_HL_r16(test_reg_code);

    uint16_t actual = env.cpu.getHL();
    uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;
    uint8_t actual_flags = env.cpu.getF();

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_wraparound(): "
                  "reg_code=%u, value",
                  test_reg_code);

    expect_eq(actual, expected, ctx);

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_wraparound(): "
                  "reg_code=%u, t_cycles",
                  test_reg_code);

    expect_eq(actual_t_cycles, expected_t_cycles, ctx);

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_wraparound(): "
                  "reg_code=%u, flags",
                  test_reg_code);

    expect_eq(actual_flags, expected_flags, ctx);
}

//------------------------------------------------------------------------------

void test_cpu_add_HL_r16_wraparound_offset() {

    TestEnv env;

    poison_state(env);
    poison_flag(env);

    char ctx[64];

    uint16_t test_value_HL = 0xFFFC;

    env.cpu.setHL(test_value_HL);

    uint8_t test_reg_code = 0b00;
    uint16_t test_value_r16 = 0x5;

    env.cpu.set_r16rp(test_reg_code, test_value_r16);

    uint16_t expected = test_value_HL + test_value_r16;
    uint16_t cycles_before = env.cpu.cycles();
    uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4
    uint8_t expected_flags = 0b00110000;

    env.cpu.add_HL_r16(test_reg_code);

    uint16_t actual = env.cpu.getHL();
    uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;
    uint8_t actual_flags = env.cpu.getF();

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_wraparound_offset(): "
                  "reg_code=%u, value",
                  test_reg_code);

    expect_eq(actual, expected, ctx);

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_wraparound_offset(): "
                  "reg_code=%u, t_cycles",
                  test_reg_code);

    expect_eq(actual_t_cycles, expected_t_cycles, ctx);

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_wraparound_offset(): "
                  "reg_code=%u, flags",
                  test_reg_code);

    expect_eq(actual_flags, expected_flags, ctx);
}

//------------------------------------------------------------------------------

void test_cpu_add_HL_r16_half_carry() {

    TestEnv env;

    poison_state(env);
    poison_flag(env);

    char ctx[64];

    uint16_t test_value_HL = 0x0FFF;

    env.cpu.setHL(test_value_HL);

    uint8_t test_reg_code = 0b00;
    uint16_t test_value_r16 = 0x1;

    env.cpu.set_r16rp(test_reg_code, test_value_r16);

    uint16_t expected = test_value_HL + test_value_r16;
    uint16_t cycles_before = env.cpu.cycles();
    uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4
    uint8_t expected_flags = 0b00100000;

    env.cpu.add_HL_r16(test_reg_code);

    uint16_t actual = env.cpu.getHL();
    uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;
    uint8_t actual_flags = env.cpu.getF();

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_half_carry(): "
                  "reg_code=%u, value",
                  test_reg_code);

    expect_eq(actual, expected, ctx);

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_half_carry(): "
                  "reg_code=%u, t_cycles",
                  test_reg_code);

    expect_eq(actual_t_cycles, expected_t_cycles, ctx);

    std::snprintf(ctx, sizeof(ctx),
                  "test_cpu_add_hl_r16_half_carry(): "
                  "reg_code=%u, flags",
                  test_reg_code);

    expect_eq(actual_flags, expected_flags, ctx);
}

//------------------------------------------------------------------------------

void test_cpu_dec_r16() {

    for (uint8_t test_opcode = 0x09; test_opcode <= 39; test_opcode++) {

        TestEnv env;

        poison_state(env);

        char ctx[64];

        uint8_t test_reg_code = (test_opcode >> 4) & 0x03;
        uint16_t test_value = 0x1234;
        env.cpu.set_r16rp(test_reg_code, test_value);

        uint16_t expected = test_value - 1;
        uint16_t cycles_before = env.cpu.cycles();
        uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4

        env.cpu.dec_r16(test_reg_code);

        uint16_t actual = env.cpu.get_r16rp(test_reg_code);
        uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_dec_16(): "
                      "reg_code=0x%02X, value",
                      test_reg_code);

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_dec_16(): "
                      "reg_code=0x%02X, t_cycles",
                      test_reg_code);

        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_dec_r16_wraparound() {

    TestEnv env;

    poison_state(env);

    env.cpu.setBC(0x0000);

    uint16_t expected = 0xFFFF;
    uint16_t cycles_before = env.cpu.cycles();
    uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4

    env.cpu.dec_r16(0b00);

    uint16_t actual = env.cpu.getBC();
    uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;

    expect_eq(actual, expected, "test_cpu_dec_r16_wraparound(): value");
    expect_eq(actual_t_cycles, expected_t_cycles,
              "test_cpu_dec_r16_wraparound(): t_cycles");
}

//------------------------------------------------------------------------------

void test_cpu_dec_r16_byte_borrow() {

    TestEnv env;

    poison_state(env);

    env.cpu.setBC(0x0100);

    uint16_t expected = 0x00FF;
    uint16_t cycles_before = env.cpu.cycles();
    uint16_t expected_t_cycles = 4; // 8(optables) - 4(fetch) = 4

    env.cpu.dec_r16(0b00);

    uint16_t actual = env.cpu.getBC();
    uint16_t actual_t_cycles = env.cpu.cycles() - cycles_before;

    expect_eq(actual, expected, "test_cpu_dec_r16_byte_borrow(): value");
    expect_eq(actual_t_cycles, expected_t_cycles,
              "test_cpu_dec_r16_byte_borrow(): t_cycles");
}

//------------------------------------------------------------------------------

void test_cpu_rlca() {

    {
        TestEnv env;

        poison_state(env);
        poison_flag(env);

        uint8_t test_value = 0x00;
        env.cpu.set_r8(0b111, test_value);

        uint8_t expected = (test_value << 1) | (test_value >> 7);
        uint8_t expected_flags = 0x00;

        env.cpu.rlca();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_rlca(): value");
        expect_eq(actual_flags, expected_flags, "test_cpu_rlca(): value");
    }

    {
        TestEnv env;

        poison_state(env);
        poison_flag(env);

        uint8_t test_value = 0x1D;
        env.cpu.set_r8(0b111, test_value);
        env.cpu.setF(false, false, false, true);

        uint8_t expected = (test_value << 1) | (test_value >> 7);
        uint8_t expected_flags = 0x00;

        env.cpu.rlca();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_rlca(): value");
        expect_eq(actual_flags, expected_flags, "test_cpu_rlca(): value");
    }

    {
        TestEnv env;

        poison_state(env);
        poison_flag(env);

        uint8_t test_value = 0x8D;
        env.cpu.set_r8(0b111, test_value);

        uint8_t expected = (test_value << 1) | (test_value >> 7);
        uint8_t expected_flags = 0x10;

        env.cpu.rlca();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint8_t actual_flags = env.cpu.getF();

        expect_eq(actual, expected, "test_cpu_rlca(): value");
        expect_eq(actual_flags, expected_flags, "test_cpu_rlca(): value");
    }
}

//------------------------------------------------------------------------------

void test_cpu_ld_r8_r8() {

    TestEnv env;

    for (uint8_t i = 0b000; i <= 0b111; i++) {
        for (uint8_t j = 0b000; j <= 0b111; j++) {
            if (i == 6 || j == 6)
                continue;
            char ctx[64];
            std::snprintf(ctx, sizeof(ctx), "i=%u j=%u", i, j);

            uint8_t test_value = i + 0x02;

            poison_state(env);

            env.cpu.set_r8(j, test_value);
            uint8_t expected = test_value;
            uint32_t expected_t_cycles = 0;
            uint32_t cycles_before = env.cpu.cycles();

            env.cpu.ld_r8_r8(i, j);

            uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
            uint8_t actual = env.cpu.get_r8(i);

            expect_eq(actual, expected, ctx);
            expect_eq(actual_t_cycles, expected_t_cycles, ctx);
        }
    }

    // hl->r8
    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {
        if (reg_code == 6)
            continue;

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "hl->r8: reg_code=%u", reg_code);

        poison_state(env);

        env.cpu.setHL(0xC500);
        uint16_t test_hl_address = env.cpu.getHL();
        uint8_t test_value = reg_code + 0x02;
        env.bus.write(test_hl_address, test_value);

        uint8_t expected = test_value;
        uint32_t expected_t_cycles = 4;
        uint32_t cycles_before = env.cpu.cycles();

        env.cpu.ld_r8_r8(reg_code, 0b110);

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint8_t actual = env.cpu.get_r8(reg_code);

        expect_eq(actual, expected, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }

    // r8->hl
    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {
        if (reg_code != 4 && reg_code != 5 && reg_code != 6) {
            char ctx[64];
            std::snprintf(ctx, sizeof(ctx), "r8->hl: reg_code=%u", reg_code);

            poison_state(env);

            env.cpu.setHL(0xC500);
            uint8_t test_value = reg_code + 0x02;
            env.cpu.set_r8(reg_code, test_value);

            uint8_t expected = test_value;
            uint32_t expected_t_cycles = 4;
            uint32_t cycles_before = env.cpu.cycles();

            env.cpu.ld_r8_r8(0b110, reg_code);

            uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
            uint8_t actual = env.bus.read(0xC500);

            expect_eq(actual, expected, ctx);
            expect_eq(actual_t_cycles, expected_t_cycles, ctx);
        }
    }

    // h->hl
    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "h->hl");

        poison_state(env);

        env.cpu.setHL(0xC500);

        uint8_t expected = 0xC5;
        uint32_t expected_t_cycles = 4;
        uint32_t cycles_before = env.cpu.cycles();

        env.cpu.ld_r8_r8(0b110, 0b100);

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint8_t actual = env.bus.read(0xC500);

        expect_eq(actual, expected, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }

    // l->hl
    {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "l->hl");

        poison_state(env);

        env.cpu.setHL(0xC500);

        uint8_t expected = 0x00;
        uint32_t expected_t_cycles = 4;
        uint32_t cycles_before = env.cpu.cycles();

        env.cpu.ld_r8_r8(0b110, 0b101);

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint8_t actual = env.bus.read(0xC500);

        expect_eq(actual, expected, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_instructions_load() {
    test_cpu_ld_r16_n16();
    test_cpu_ld_r16mem_a();
    test_cpu_ld_a_r16mem();

    test_cpu_ld_r8_n8();
    test_cpu_ld_r8_r8();

    test_cpu_inc_r8();
    test_cpu_dec_r8();

    test_cpu_inc_r16();
    test_cpu_inc_r16_wraparound();
    test_cpu_inc_r16_byte_carry();

    test_cpu_dec_r16();
    test_cpu_dec_r16_wraparound();
    test_cpu_dec_r16_byte_borrow();

    test_cpu_add_HL_r16();
    test_cpu_add_HL_r16_wraparound();
    test_cpu_add_HL_r16_wraparound_offset();
    test_cpu_add_HL_r16_half_carry();

    test_cpu_rlca();
}

//------------------------------------------------------------------------------

void test_cpu_decode_ld_r16_n16() {

    TestEnv env;

    for (uint8_t test_opcode = 0x01; test_opcode <= 0x31; test_opcode += 0x10) {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "test_opcode=0x%02X", test_opcode);

        env.cpu.setPC(0xC000);

        uint8_t test_reg_code = (test_opcode >> 4) & 0x07;
        uint16_t test_pc = env.cpu.getPC();
        uint16_t test_value = 0x1234;
        env.bus.write(test_pc, test_opcode);
        env.bus.write(test_pc + 0x02, (test_value >> 8));
        env.bus.write(test_pc + 0x01, test_value);

        poison_state(env);
        poison_flag(env);

        uint8_t expected_f = env.cpu.getF();
        uint16_t expected = test_value;
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];
        uint32_t cycles_before = env.cpu.cycles();

        env.cpu.decode();

        uint16_t actual = env.cpu.get_r16rp(test_reg_code);
        uint8_t actual_f = env.cpu.getF();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected, ctx);
        expect_eq(actual_f, expected_f, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_ld_r16mem_a() {

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_opcode = 0x02;
        env.bus.write(test_pc, test_opcode);

        env.cpu.setAF(0x1200);

        uint8_t test_value = env.cpu.getAF() >> 8;
        uint16_t test_address = 0xC500;
        env.cpu.setBC(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.bus.read(test_address);
        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_r16mem_a(): a->bc value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_r16mem_a(): a->bc pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_r16mem_a(): a->bc t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_opcode = 0x12;
        env.bus.write(test_pc, test_opcode);

        env.cpu.setAF(0x2300);

        uint8_t test_value = env.cpu.getAF() >> 8;
        uint16_t test_address = 0xC500;
        env.cpu.setDE(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.bus.read(test_address);
        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_r16mem_a(): a->de value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_r16mem_a(): a->de pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_r16mem_a(): a->de t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_opcode = 0x22;
        env.bus.write(test_pc, test_opcode);
        env.cpu.setAF(0x3400);
        uint8_t test_value = env.cpu.getAF() >> 8;
        uint16_t test_address = 0xC500;
        env.cpu.setHL(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.bus.read(test_address);
        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_r16mem_a(): a->hl_inc value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_r16mem_a(): a->hl_inc pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_r16mem_a(): a->hl_inc t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_opcode = 0x32;
        env.bus.write(test_pc, test_opcode);
        env.cpu.setAF(0x4500);
        uint8_t test_value = env.cpu.getAF() >> 8;
        uint16_t test_address = 0xC500;
        env.cpu.setHL(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.bus.read(test_address);
        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_r16mem_a(): a->hl_dec value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_r16mem_a(): a->hl_dec pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_r16mem_a(): a->hl_dec t_cycles");
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_ld_a_r16mem() {

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint16_t test_opcode = 0x0A;

        env.bus.write(test_pc, test_opcode);

        uint16_t test_address = 0xC500;
        uint8_t test_value = 0x12;
        env.bus.write(test_address, test_value);
        env.cpu.setBC(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint16_t actual_pc = test_pc + 1;
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_a_r16mem(): bc->a value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_a_r16mem(): bc->a pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_a_r16mem(): bc->a t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint16_t test_opcode = 0x1A;

        env.bus.write(test_pc, test_opcode);

        uint16_t test_address = 0xC500;
        uint8_t test_value = 0x12;
        env.bus.write(test_address, test_value);
        env.cpu.setDE(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint16_t actual_pc = test_pc + 1;
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_a_r16mem(): de->a value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_a_r16mem(): de->a pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_a_r16mem(): de->a t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint16_t test_opcode = 0x2A;

        env.bus.write(test_pc, test_opcode);

        uint16_t test_address = 0xC500;
        uint8_t test_value = 0x12;
        env.bus.write(test_address, test_value);
        env.cpu.setHL(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint16_t actual_pc = test_pc + 1;
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_a_r16mem(): hl_inc->a value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_a_r16mem(): hl_inc->a pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_a_r16mem(): hl_inc->a t_cycles");
    }

    {
        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint16_t test_opcode = 0x3A;

        env.bus.write(test_pc, test_opcode);

        uint16_t test_address = 0xC500;
        uint8_t test_value = 0x12;
        env.bus.write(test_address, test_value);
        env.cpu.setHL(test_address);

        uint8_t expected = test_value;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint8_t actual = env.cpu.get_r8(0b111);
        uint16_t actual_pc = test_pc + 1;
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        expect_eq(actual, expected,
                  "test_cpu_decode_ld_a_r16mem(): hl_dec->a value");
        expect_eq(actual_pc, expected_pc,
                  "test_cpu_decode_ld_a_r16mem(): hl_dec->a pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_ld_a_r16mem(): hl_dec->a t_cycles");
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_inc_r16() {

    for (uint8_t test_opcode = 0x03; test_opcode <= 0x33; test_opcode += 0x10) {

        TestEnv env;

        char ctx[64];

        poison_state(env);

        env.cpu.setPC(0xC000);

        uint16_t test_pc = env.cpu.getPC();

        env.bus.write(test_pc, test_opcode);

        uint8_t test_reg_code = (test_opcode >> 4) & 0x03;
        uint16_t test_value = 0x1234;

        env.cpu.set_r16rp(test_reg_code, test_value);

        uint16_t expected = test_value + 1;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint16_t actual = env.cpu.get_r16rp(test_reg_code);
        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_inc_r16(): value, "
                      "reg_code=%0x02X",
                      test_reg_code);

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_inc_r16(): pc, "
                      "reg_code=%0x02X",
                      test_reg_code);

        expect_eq(actual_pc, expected_pc, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_inc_r16(): t_cycles, "
                      "reg_code=%0x02X",
                      test_reg_code);

        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_inc_r8() {

    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {

        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_opcode = (reg_code << 3) | 0x04;

        env.bus.write(test_pc, test_opcode);

        uint8_t test_value = reg_code + 0x02;
        uint16_t test_address = 0xC500;

        if (reg_code == 0b110) {
            env.cpu.setHL(test_address);
        }

        env.cpu.set_r8(reg_code, test_value);

        uint8_t expected = static_cast<uint8_t>(test_value + 1);
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        // env.cpu.inc_r8(reg_code);
        env.cpu.decode();

        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint8_t actual = env.cpu.get_r8(reg_code);

        expect_eq(actual, expected, "test_cpu_decode_inc_r8(): value");
        expect_eq(actual_pc, expected_pc, "test_cpu_decode_inc_r8(): pc");

        char ctx[64];
        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_inc_r8(): "
                      "t_cycles, opcode=%02X",
                      test_opcode);

        expect_eq(actual_t_cycles, expected_t_cycles, ctx);

        if (reg_code == 0b110) {
            expect_eq(env.cpu.getHL(), test_address,
                      "test_cpu_decode_inc_r8(): modified HL!");
        }
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_dec_r8() {

    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {

        TestEnv env;

        poison_state(env);

        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        uint8_t test_opcode = (reg_code << 3) | 0x05;

        env.bus.write(test_pc, test_opcode);

        uint8_t test_value = reg_code + 0x02;
        uint16_t test_address = 0xC500;

        if (reg_code == 0b110) {
            env.cpu.setHL(test_address);
        }

        env.cpu.set_r8(reg_code, test_value);

        uint8_t expected = test_value - 1;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint8_t actual = env.cpu.get_r8(reg_code);
        uint16_t actual_pc = env.cpu.getPC();

        expect_eq(actual, expected, "test_cpu_decode_dec_r8(): value");
        expect_eq(actual_pc, expected_pc, "test_cpu_decode_dec_r8(): pc");
        expect_eq(actual_t_cycles, expected_t_cycles,
                  "test_cpu_decode_dec_r8(): t_cycles");

        if (reg_code == 0b110) {
            expect_eq(env.cpu.getHL(), test_address,
                      "test_cpu_decode_dec_r8(): modified HL!");
        }
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_ld_r8_n8() {

    TestEnv env;

    for (uint8_t reg_code = 0b000; reg_code <= 0b111; reg_code++) {
        char ctx[64];
        std::snprintf(ctx, sizeof(ctx), "reg_code <= 0x%2X", reg_code);
        uint8_t test_opcode = (reg_code << 3) | 0x06;
        uint8_t test_value = reg_code + 0x10;
        env.cpu.setPC(0xC000);
        uint16_t test_pc = env.cpu.getPC();
        env.bus.write(test_pc, test_opcode);
        env.bus.write(test_pc + 1, test_value);

        poison_state(env);
        poison_flag(env);

        uint16_t test_address = 0xC500;

        if (reg_code == 0b110) {
            env.cpu.setHL(test_address);
        }

        uint8_t expected = test_value;
        uint8_t expected_f = env.cpu.getF();
        uint16_t expected_pc = test_pc + 2;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;
        uint16_t actual_pc = env.cpu.getPC();
        uint8_t actual = env.cpu.get_r8(reg_code);
        uint8_t actual_f = env.cpu.getF();

        expect_eq(actual, expected, ctx);
        expect_eq(actual_f, expected_f, ctx);
        expect_eq(actual_pc, expected_pc, ctx);
        expect_eq(actual_t_cycles, expected_t_cycles, ctx);

        if (reg_code == 0b110) {
            expect_eq(env.cpu.getHL(), test_address,
                      "test_cpu_decode_ld_r8_n8(): modified HL!");
        }
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_dec_r16() {

    for (uint8_t test_opcode = 0x0B; test_opcode <= 0x3B; test_opcode += 0x10) {

        TestEnv env;

        poison_state(env);

        char ctx[64];

        env.cpu.setPC(0xC000);

        uint16_t test_pc = env.cpu.getPC();
        env.bus.write(test_pc, test_opcode);

        uint8_t test_reg_code = (test_opcode >> 4) & 0x03;
        uint16_t test_value = 0x1234;

        env.cpu.set_r16rp(test_reg_code, test_value);

        uint16_t expected = test_value - 1;
        uint16_t expected_pc = test_pc + 1;
        uint32_t cycles_before = env.cpu.cycles();
        uint32_t expected_t_cycles = OPCODE_CYCLES[test_opcode];

        env.cpu.decode();

        uint16_t actual = env.cpu.get_r16rp(test_reg_code);
        uint16_t actual_pc = env.cpu.getPC();
        uint32_t actual_t_cycles = env.cpu.cycles() - cycles_before;

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_dec_r16(): "
                      "reg_code=%u, value",
                      test_reg_code);

        expect_eq(actual, expected, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_dec_r16(): "
                      "reg_code=%u, pc",
                      test_reg_code);

        expect_eq(actual_pc, expected_pc, ctx);

        std::snprintf(ctx, sizeof(ctx),
                      "test_cpu_decode_dec_r16(): "
                      "reg_code=%u, t_cycles",
                      test_reg_code);

        expect_eq(actual_t_cycles, expected_t_cycles, ctx);
    }
}

//------------------------------------------------------------------------------

void test_cpu_decode_ld_r8_r8() {
    TestEnv env;

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
                        uint16_t hl_before = env.cpu.getHL();
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
                        expect_eq(env.cpu.getHL(), hl_before,
                                  "test_cpu_decode_ld_r8_r8(): modified HL!");
                    } else if (src == 0b101) { // LD [HL], L
                        env.cpu.setHL(0xC500);
                        uint16_t hl_before = env.cpu.getHL();
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
                        expect_eq(env.cpu.getHL(), hl_before,
                                  "test_cpu_decode_ld_r8_r8(): modified HL!");
                    } else { // LD [HL], r8
                        env.cpu.setHL(0xC500);
                        uint16_t hl_before = env.cpu.getHL();
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
                        expect_eq(env.cpu.getHL(), hl_before,
                                  "test_cpu_decode_ld_r8_r8(): modified HL!");
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

void test_cpu_decode() {
    test_cpu_decode_ld_r16_n16();
    test_cpu_decode_ld_r16mem_a();
    test_cpu_decode_ld_a_r16mem();
    test_cpu_decode_inc_r16();
    test_cpu_decode_dec_r16();
    test_cpu_decode_ld_r8_r8();
    test_cpu_decode_ld_r8_n8();
    test_cpu_decode_inc_r8();
    test_cpu_decode_dec_r8();
}

//------------------------------------------------------------------------------

int run_cpu_tests() {
    test_cpu_register_pairs();
    test_cpu_get_flag_z();
    test_cpu_get_flag_n();
    test_cpu_get_flag_h();
    test_cpu_get_flag_c();
    test_cpu_opcode_field_decoding();
    test_cpu_set_r8();
    test_cpu_set_r8_hl_indirect();
    test_cpu_fetch_basic();
    test_cpu_decode();
    test_cpu_get_n8();
    test_cpu_get_n16();
    test_cpu_set_r16mem();
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
