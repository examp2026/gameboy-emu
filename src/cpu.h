#pragma once
#include "bus.h"
#include <cstdint>
#include <memory>

#include <iostream>

class CPU {
  public:
    CPU(Bus &bus) : bus(bus) {}

    CPU(const CPU &) = delete;
    CPU &operator=(const CPU &) = delete;
    CPU(CPU &&) = delete;
    CPU &operator=(CPU &&) = delete;

    uint8_t read_byte(uint16_t address);
    void write_byte(uint16_t address, uint8_t value);

    void set_r8(uint8_t reg_code, uint8_t value);
    uint8_t get_r8(uint8_t reg_code);
    uint16_t get_r16rp(uint8_t reg_code);
    void set_r16rp(uint8_t reg_code, uint16_t value);
    uint16_t get_r16rp2(uint8_t reg_code);
    void set_r16rp2(uint8_t reg_code, uint16_t value);
    uint8_t get_r16mem(uint8_t reg_code);
    void set_r16mem(uint8_t reg_code, uint8_t value);

    uint8_t decode_r8_dest(uint8_t opcode);
    uint8_t decode_r8_source(uint8_t opcode);
    uint8_t decode_r16_dest(uint8_t opcode);

    uint8_t get_n8();
    uint16_t get_n16();

    uint16_t getBC() const;
    uint16_t getHL() const;
    uint16_t getDE() const;
    uint16_t getAF() const;
    uint16_t getSP() const;
    uint16_t getPC() const;

    uint8_t getF();
    void setF(uint8_t value);
    void setF(bool z, bool n, bool h, bool c);

    bool get_flag_z();
    bool get_flag_n();
    bool get_flag_h();
    bool get_flag_c();

    void setBC(uint16_t value);
    void setDE(uint16_t value);
    void setHL(uint16_t value);
    void setAF(uint16_t value);

    void setSP(uint16_t pointer);
    void setPC(uint16_t pointer);

    void ld_r8_r8(uint8_t reg_code_l, uint8_t reg_code_r);
    void ld_r8_n8(uint8_t reg_code_l);

    void ld_r16_n16(uint8_t reg_code);
    void ld_r16mem_a(uint8_t reg_code);
    void ld_a_r16mem(uint8_t reg_code);

    void inc_r8(uint8_t reg_code);
    void dec_r8(uint8_t reg_code);

    void inc_r16(uint8_t reg_code);
    void dec_r16(uint8_t reg_code);

    void tick(uint16_t delta);
    void internal_cycle();

    uint32_t cycles();

    uint8_t fetch();
    void decode();

  private:
    uint8_t A{};
    uint8_t B{}, C{};
    uint8_t D{}, E{};
    uint8_t H{}, L{};
    uint8_t F{};
    uint16_t sp{};
    uint16_t pc{};
    uint32_t t_cycles{};
    static constexpr uint32_t t_per_m_cycle = 4;

    Bus &bus;
};
