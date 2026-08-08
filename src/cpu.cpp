#include "cpu.h"

//------------------------------------------------------------------------------

void CPU::setBC(uint16_t value)
{
    B = static_cast<uint8_t>(value >> 8);
    C = static_cast<uint8_t>(value & 0x00FF);
}

//------------------------------------------------------------------------------

void CPU::setDE(uint16_t value)
{
    D = static_cast<uint8_t>(value >> 8);
    E = static_cast<uint8_t>(value & 0x00FF);
}

//------------------------------------------------------------------------------

void CPU::setHL(uint16_t value)
{
    H = static_cast<uint8_t>(value >> 8);
    L = static_cast<uint8_t>(value & 0x00FF);
}

//------------------------------------------------------------------------------

void CPU::setF(uint8_t value)
{
    F = value & 0xF0;
}

//------------------------------------------------------------------------------

void CPU::setAF(uint16_t value)
{
    A = static_cast<uint8_t>(value >> 8);
    setF(static_cast<uint8_t>(value));
}

//------------------------------------------------------------------------------

uint16_t CPU::getBC() const
{
    uint16_t register_pair = (B << 8) | C;
    return register_pair;
}

//------------------------------------------------------------------------------

uint16_t CPU::getDE() const
{
    uint16_t register_pair = (D << 8) | E;
    return register_pair;
}

//------------------------------------------------------------------------------

uint16_t CPU::getHL() const
{
    uint16_t register_pair = (H << 8) | L;
    return register_pair;
}

//------------------------------------------------------------------------------

uint16_t CPU::getAF() const
{
    uint16_t register_pair = (A << 8) | F;
    return register_pair;
}

//------------------------------------------------------------------------------

void CPU::setSP(uint16_t pointer)
{
    sp = pointer;
}

//------------------------------------------------------------------------------

uint16_t CPU::getSP() const
{
    return sp;
}

//------------------------------------------------------------------------------

void CPU::setPC(uint16_t pointer)
{
    pc = pointer;
}

//------------------------------------------------------------------------------

uint16_t CPU::getPC() const
{
    return pc;
}

//------------------------------------------------------------------------------

uint8_t CPU::get_r8_dest(uint8_t opcode)
{
    //[0][0][D][S][T][0][0][0]
    uint8_t dest = (opcode >> 3) & 0x07;
    return dest;
}

//------------------------------------------------------------------------------

uint8_t CPU::get_r8_source(uint8_t opcode)
{
    //[0][0][0][0][0][S][R][C]
    uint8_t source = opcode & 0x07;
    return source;
}

//------------------------------------------------------------------------------

void CPU::set_r8(uint8_t dest, uint8_t value)
{
    switch(dest)
    {
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
	bus.write(getHL(), value);
	break;
    case 0b111:
	A = value;
	break;
    default:
	break;
    }
}

//------------------------------------------------------------------------------

uint8_t CPU::get_r8(uint8_t reg_code)
{
    switch(reg_code)
    {
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
    case 0b110:
    {	//[HL]
	uint8_t byte = bus.read(getHL());
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

uint8_t CPU::get_r16_dest(uint8_t opcode)
{
    //[0][0][de][st][0][0][0][0] => [0][0][0][0][0][0][de][st]
    uint8_t dest = (opcode >> 4) & 0x03;
    return dest;
}

//------------------------------------------------------------------------------

uint16_t CPU::get_r16rp(uint8_t reg_code)
{
    uint16_t bytes {};
    
    switch(reg_code){
    case 0b00:
    {
	bytes = getBC();
	return bytes;
    }
    case 0b01:
    {
	bytes = getDE();
	return bytes;
    }
    case 0b10:
    {
	bytes = getHL();
	return bytes;
    }
    case 0b11:
    {
	bytes = getSP();
	return bytes;
    }
    default:
	break;
    }

    return 0;
}

//------------------------------------------------------------------------------

uint16_t CPU::get_r16rp2(uint8_t reg_code)
{
    uint16_t bytes {};

    switch(reg_code){
    case 0b00:
    {
	bytes = getBC();
	return bytes;
    }
    case 0b01:
    {
	bytes = getDE();
	return bytes;
    }
    case 0b10:
    {
	bytes = getHL();
	return bytes;
    }
    case 0b11:
    {
	bytes = getAF();
	return bytes;
    }
    default:
	break;
    }
}

//------------------------------------------------------------------------------

uint8_t CPU::get_r16mem(uint8_t reg_code)
{
    switch(reg_code){
    case 0b00:
    {
	uint8_t byte = bus.read(getBC());
        return byte;
    }
    case 0b01:
    {
	uint8_t byte = bus.read(getDE());
	return byte;
    }
    case 0b10:
    {
	uint8_t byte = bus.read(getHL());
	return byte;
    }
    case 0b11:
    {
	uint8_t byte = bus.read(getAF());
	return byte;
    }
    default:
	break;
    }

    return 0;
}

//--------------------[INSTRUCTIONS SECTION]------------------------------------

void CPU::ld_r8_r8(uint8_t reg_code_l, uint8_t reg_code_r)
{
    uint8_t value = get_r8(reg_code_r);
    set_r8(reg_code_l, value);
}

//------------------------------------------------------------------------------

uint8_t CPU::fetch()
{
    uint8_t byte = bus.read(pc);
    pc += 1;
    return byte;
}

//--------------------[UNTESTED]------------------------------------------------

uint8_t CPU::decode()
{
    uint8_t opcode = fetch();
    uint8_t dest {};
    uint8_t source {};

    if(opcode >= 0x00 && opcode <= 0x3F ){
	if((opcode & 0x0F) == 0x02){
	    //LD [r16], A
	}
	//LD r8,n8 column
	if((opcode & 0x0F) == 0x06){
	    //LD r8, n8
	}
	if((opcode & 0x0F) == 0x0A){
	    //LD r8, [r16]
	}
	return 0;
    }
    if(opcode >= 0x40 && opcode <= 0x7F){
	if(opcode == 0x76){
	    // HALT	   
	}else{
	    dest = get_r8_dest(opcode);
	    source = get_r8_source(opcode);
	    set_r8(dest, get_r8(source));
	}

	return 0;
    }

    return 0;
    
}
