#include "cpu.h"

//------------------------------------------------------------------------------

void CPU::setBC(uint16_t value) {
    B = static_cast<uint8_t>(value >> 8);
    C = static_cast<uint8_t>(value & 0x00FF);
}

//------------------------------------------------------------------------------

void CPU::setDE(uint16_t value) {
    D = static_cast<uint8_t>(value >> 8);
    E = static_cast<uint8_t>(value & 0x00FF);
}

//------------------------------------------------------------------------------

void CPU::setHL(uint16_t value) {
    H = static_cast<uint8_t>(value >> 8);
    L = static_cast<uint8_t>(value & 0x00FF);
}

//------------------------------------------------------------------------------

void CPU::setF(uint8_t value) { F = value & 0xF0; }

//------------------------------------------------------------------------------

uint8_t CPU::getF() { return F; }

//------------------------------------------------------------------------------

void CPU::setAF(uint16_t value) {
    A = static_cast<uint8_t>(value >> 8);
    setF(static_cast<uint8_t>(value));
}

//------------------------------------------------------------------------------

uint16_t CPU::getBC() const {
    uint16_t register_pair = (B << 8) | C;
    return register_pair;
}

//------------------------------------------------------------------------------

uint16_t CPU::getDE() const {
    uint16_t register_pair = (D << 8) | E;
    return register_pair;
}

//------------------------------------------------------------------------------

uint16_t CPU::getHL() const {
    uint16_t register_pair = (H << 8) | L;
    return register_pair;
}

//------------------------------------------------------------------------------

uint16_t CPU::getAF() const {
    uint16_t register_pair = (A << 8) | F;
    return register_pair;
}

//------------------------------------------------------------------------------

void CPU::setSP(uint16_t pointer) { sp = pointer; }

//------------------------------------------------------------------------------

uint16_t CPU::getSP() const { return sp; }

//------------------------------------------------------------------------------

void CPU::setPC(uint16_t pointer) { pc = pointer; }

//------------------------------------------------------------------------------

uint16_t CPU::getPC() const { return pc; }

//------------------------------------------------------------------------------

uint8_t CPU::read_byte(uint16_t address) {
    tick(4);
    uint8_t byte = bus.read(address);
    return byte;
}

//------------------------------------------------------------------------------

void CPU::write_byte(uint16_t address, uint8_t value) {
    tick(4);
    bus.write(address, value);
}

//------------------------------------------------------------------------------

uint8_t CPU::decode_r8_dest(uint8_t opcode) {
    //[0][0][D][S][T][0][0][0]
    uint8_t dest_reg_code = (opcode >> 3) & 0x07;
    return dest_reg_code;
}

//------------------------------------------------------------------------------

uint8_t CPU::decode_r8_source(uint8_t opcode) {
    //[0][0][0][0][0][S][R][C]
    uint8_t source = opcode & 0x07;
    return source;
}

//------------------------------------------------------------------------------

void CPU::set_r8(uint8_t reg_code, uint8_t value) {
    switch (reg_code) {
    case 0b000:
        B = value;
        break;
    case 0b001:
        C = value;
        break;
    case 0b010:
        D = value;
        break;
    case 0b011:
        E = value;
        break;
    case 0b100:
        H = value;
        break;
    case 0b101:
        L = value;
        break;
    case 0b110:
        // bus.write(getHL(), value);
        write_byte(getHL(), value);
        break;
    case 0b111:
        A = value;
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------

uint8_t CPU::get_r8(uint8_t reg_code) {
    switch (reg_code) {
    case 0b000:
        return B;
    case 0b001:
        return C;
    case 0b010:
        return D;
    case 0b011:
        return E;
    case 0b100:
        return H;
    case 0b101:
        return L;
    case 0b110: { //[HL]
                  // uint8_t byte = bus.read(getHL());
        uint8_t byte = read_byte(getHL());
        return byte;
    }
    case 0b111:
        return A;
    default:
        break;
    }

    return 0;
}

//------------------------------------------------------------------------------

uint8_t CPU::get_n8() {
    uint8_t byte = bus.read(pc);
    pc++;
    return byte;
}

//------------------------------------------------------------------------------

uint8_t CPU::decode_r16_dest(uint8_t opcode) {
    //[0][0][de][st][0][0][0][0] => [0][0][0][0][0][0][de][st]
    uint8_t dest = (opcode >> 4) & 0x03;
    return dest;
}

//------------------------------------------------------------------------------

uint16_t CPU::get_r16rp(uint8_t reg_code) {
    uint16_t bytes{};

    switch (reg_code) {
    case 0b00: {
        bytes = getBC();
        return bytes;
    }
    case 0b01: {
        bytes = getDE();
        return bytes;
    }
    case 0b10: {
        bytes = getHL();
        return bytes;
    }
    case 0b11: {
        bytes = getSP();
        return bytes;
    }
    default:
        break;
    }

    return 0;
}

//------------------------------------------------------------------------------

uint16_t CPU::get_r16rp2(uint8_t reg_code) {
    uint16_t bytes{};

    switch (reg_code) {
    case 0b00: {
        bytes = getBC();
        return bytes;
    }
    case 0b01: {
        bytes = getDE();
        return bytes;
    }
    case 0b10: {
        bytes = getHL();
        return bytes;
    }
    case 0b11: {
        bytes = getAF();
        return bytes;
    }
    default:
        break;
    }

    return 0;
}

//------------------------------------------------------------------------------

void CPU::set_r16rp(uint8_t reg_code, uint16_t value) {
    switch (reg_code) {
    case 0b00: {
        setBC(value);
        break;
    }
    case 0b01: {
        setDE(value);
        break;
    }
    case 0b10: {
        setHL(value);
        break;
    }
    case 0b11: {
        setSP(value);
        break;
    }
    default:
        break;
    }
}

//------------------------------------------------------------------------------

void CPU::set_r16rp2(uint8_t reg_code, uint16_t value) {
    switch (reg_code) {
    case 0b00: {
        setBC(value);
        break;
    }
    case 0b01: {
        setDE(value);
        break;
    }
    case 0b10: {
        setHL(value);
        break;
    }
    case 0b11: {
        setAF(value);
        break;
    }
    default:
        break;
    }
}

//------------------------------------------------------------------------------

uint8_t CPU::get_r16mem(uint8_t reg_code) {
    switch (reg_code) {
    case 0b00: {
        uint8_t byte = bus.read(getBC());
        return byte;
    }
    case 0b01: {
        uint8_t byte = bus.read(getDE());
        return byte;
    }
    case 0b10: {
        uint8_t byte = bus.read(getHL());
        uint16_t bytes = getHL() + 1;
        setHL(bytes);
        return byte;
    }
    case 0b11: {
        uint8_t byte = bus.read(getHL());
        uint16_t bytes = getHL() - 1;
        setHL(bytes);
        return byte;
    }
    default:
        break;
    }

    return 0;
}

//------------------------------------------------------------------------------

uint16_t CPU::get_n16() {
    uint8_t low = get_n8();
    uint8_t high = get_n8();
    uint16_t bytes =
	(static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
    return bytes;
}

//--------------------[INSTRUCTIONS SECTION]------------------------------------

void CPU::ld_r8_r8(uint8_t reg_code_l, uint8_t reg_code_r) {
    uint8_t value = get_r8(reg_code_r);
    set_r8(reg_code_l, value);
}

//------------------------------------------------------------------------------

void CPU::ld_r8_n8(uint8_t reg_code_l) {
    uint8_t value = get_n8();
    set_r8(reg_code_l, value);
}

//--------------------[IN-PROGRESS]---------------------------------------------

void CPU::ld_r16_n16(uint8_t reg_code) {
    uint16_t bytes = get_n16();
    set_r16rp(reg_code, bytes);
}

//------------------------------------------------------------------------------

uint32_t CPU::cycles() {
    return t_cycles;
}

//------------------------------------------------------------------------------

void CPU::tick(uint16_t delta) {
    t_cycles += delta;
}

//------------------------------------------------------------------------------


uint8_t CPU::fetch() {
    uint8_t byte = read_byte(pc);
    pc += 1;
    return byte;
}

//--------------------[UNTESTED]------------------------------------------------

void CPU::decode() {
    uint8_t opcode = fetch();
    uint8_t dest_reg_code{};
    uint8_t src_reg_code{};

    // ld_r16_n16()
    switch ((opcode)) {
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31: {
        dest_reg_code = decode_r16_dest(opcode);
        ld_r16_n16(dest_reg_code);
        break;
    }
    default:
        break;
    }

    // ld_r8_r8()
    if ((opcode & 0xC0) == 0x40) {
        // if(opcode = 0x76){
        //     // return halt();
        // }
        dest_reg_code = decode_r8_dest(opcode);
        src_reg_code = decode_r8_source(opcode);
        ld_r8_r8(dest_reg_code, src_reg_code);
    }
}
