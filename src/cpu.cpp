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

void CPU::setF(uint8_t value) {
    F = value & 0xF0;
}

//------------------------------------------------------------------------------

void CPU::setF(bool z, bool n, bool h, bool c) {
    uint8_t value =
	static_cast<uint8_t>((z << 7) | (n << 6) | (h << 5) | (c << 4));
    setF(value);
}

//------------------------------------------------------------------------------

uint8_t CPU::getF() { return F; }

//------------------------------------------------------------------------------

bool CPU::get_flag_z() {
    return (getF() & 0x80) != 0;
}

//------------------------------------------------------------------------------

bool CPU::get_flag_n() {
    return (getF() & 0x40) != 0;
}

//------------------------------------------------------------------------------

bool CPU::get_flag_h() {
    return (getF() & 0x20) != 0;
}

//------------------------------------------------------------------------------

bool CPU::get_flag_c() {
    return (getF() & 0x10) != 0;
}

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
    // uint8_t byte = bus.read(pc);
    uint8_t byte = read_byte(pc);
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

void CPU::set_r16mem(uint8_t reg_code, uint8_t value) {
    
    switch (reg_code) {
    case 0b00: {
	uint16_t address = getBC();
	write_byte(address, value);
	break;
    }
    case 0b01: {
	uint16_t address = getDE();
	write_byte(address, value);
	break;
    }
    case 0b10: {
	uint16_t address = getHL();
	write_byte(address, value);
	setHL(getHL() + 1);
	break;
    }
    case 0b11: {
	uint16_t address = getHL();
	write_byte(address, value);
	setHL(getHL() - 1);
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
        uint8_t byte = read_byte(getBC()); // bus.read(getBC());
        return byte;
    }
    case 0b01: {
        uint8_t byte = read_byte(getDE()); // bus.read(getDE());
        return byte;
    }
    case 0b10: {
        uint8_t byte = read_byte(getHL());
        uint16_t bytes = getHL() + 1;
        setHL(bytes);
        return byte;
    }
    case 0b11: {
        uint8_t byte = read_byte(getHL()); // bus.read(getHL());
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

void CPU::ld_r16_n16(uint8_t reg_code) {
    uint16_t bytes = get_n16();
    set_r16rp(reg_code, bytes);
}

//------------------------------------------------------------------------------

void CPU::ld_r16mem_a(uint8_t reg_code) {
    uint8_t value = A;
    set_r16mem(reg_code, value);
}

//------------------------------------------------------------------------------

void CPU::inc_r16(uint8_t reg_code) {
    internal_cycle();
    uint16_t value = get_r16rp(reg_code);
    value++;
    set_r16rp(reg_code, value);
}

//------------------------------------------------------------------------------

void CPU::inc_r8(uint8_t reg_code) {
    uint8_t value = get_r8(reg_code);
    uint8_t result = value + 1;

    bool z = (result == 0);
    bool n = false;
    bool h = ((value & 0x0F) == 0x0F);
    bool c = get_flag_c();

    set_r8(reg_code, result);
    setF(z, n, h, c);
}

//------------------------------------------------------------------------------

void CPU::dec_r8(uint8_t reg_code) {
    uint8_t value = get_r8(reg_code);
    uint8_t result = value - 1;

    bool z = (result == 0);
    bool n = true;
    bool h = ((value & 0x0F) == 0x00);
    bool c = get_flag_c();

    set_r8(reg_code, result);
    setF(z, n, h, c);
}

//------------------------------------------------------------------------------

void CPU::ld_a_r16mem(uint8_t reg_code) {
    uint8_t value = get_r16mem(reg_code); // t_cycles + 4
    A = value;
}

//------------------------------------------------------------------------------

void CPU::ld_r8_r8(uint8_t reg_code_l, uint8_t reg_code_r) {
    uint8_t value = get_r8(reg_code_r);
    set_r8(reg_code_l, value);
}

//------------------------------------------------------------------------------

void CPU::ld_r8_n8(uint8_t reg_code_l) {
    uint8_t value = get_n8();
    set_r8(reg_code_l, value);
}

//------------------------------------------------------------------------------

void CPU::add_HL_r16(uint8_t reg_code) {
    internal_cycle();
    uint16_t HL_value = getHL();
    uint16_t r16_value = get_r16rp(reg_code);
    uint32_t result = static_cast<uint32_t>(HL_value) + r16_value;

    bool z = get_flag_z();
    bool n = false;
    bool h = ((HL_value & 0x0FFF) + (r16_value & 0x0FFF)) > 0x0FFF;
    bool c = result > 0xFFFF;

    setHL(static_cast<uint16_t>(result));
    setF(z, n, h, c);
}

//------------------------------------------------------------------------------

void CPU::dec_r16(uint8_t reg_code) {
    internal_cycle();
    uint16_t value = get_r16rp(reg_code);
    value--;
    set_r16rp(reg_code, value);
}

//------------------------------------------------------------------------------

uint32_t CPU::cycles() { return t_cycles; }

//------------------------------------------------------------------------------

void CPU::tick(uint16_t delta) { t_cycles += delta; }

//------------------------------------------------------------------------------

void CPU::internal_cycle() { tick(t_per_m_cycle); }

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
    switch (opcode) {
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
    
    // ld_r16mem_a
    switch(opcode) {
    case 0x02:
    case 0x12:
    case 0x22:
    case 0x32:
	dest_reg_code = decode_r16_dest(opcode);
	ld_r16mem_a(dest_reg_code);
	break;
    default:
	break;
    }

    // inc_r16

    switch(opcode) {
    case 0x03:
    case 0x13:
    case 0x23:
    case 0x33:
	dest_reg_code = decode_r16_dest(opcode);
	inc_r16(dest_reg_code);
	break;
    default:
	break;
    }


    //ld_a_r16mem
    switch(opcode) {
    case 0x0A:
    case 0x1A:
    case 0x2A:
    case 0x3A:
	dest_reg_code = decode_r16_dest(opcode);
	ld_a_r16mem(dest_reg_code);
	break;
    default:
	break;
    }

    // inc_r8
    switch (opcode) {
    case 0x04:
    case 0x14:
    case 0x24:
    case 0x34:
    case 0x0C:
    case 0x1C:
    case 0x2C:
    case 0x3C: {
	dest_reg_code = decode_r8_dest(opcode);
	inc_r8(dest_reg_code);
	break;
    }
    default:
	break;
    }

    // inc_r8
    switch (opcode) {
    case 0x05:
    case 0x15:
    case 0x25:
    case 0x35:
    case 0x0D:
    case 0x1D:
    case 0x2D:
    case 0x3D: {
	dest_reg_code = decode_r8_dest(opcode);
	dec_r8(dest_reg_code);
	break;
    }
    default:
	break;
    }
    
    // ld_r8_n8
    switch(opcode) {
    case 0x06:
    case 0x16:
    case 0x26:
    case 0x36:
    case 0x0E:
    case 0x1E:
    case 0x2E:
    case 0x3E:{
	dest_reg_code = decode_r8_dest(opcode);
	ld_r8_n8(dest_reg_code);
	// there must be break;
    }
    default:
	break;
    }

    // add_hl_r16
    switch(opcode) {
    case 0x09:
    case 0x19:
    case 0x29:
    case 0x39:
	break;
    default:
	break;
    }

    // dec_r16
    switch(opcode) {
    case 0x0B:
    case 0x1B:
    case 0x2B:
    case 0x3B:
	dest_reg_code = decode_r16_dest(opcode);
	dec_r16(dest_reg_code);
	break;
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
