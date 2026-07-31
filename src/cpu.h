#pragma once
#include <cstdint>

class CPU
{
 public:
  CPU() = default;
  
  uint16_t getA() const;
  uint16_t getB() const;
  uint16_t getC() const;
  uint16_t getD() const;
  uint16_t getE() const;
  uint16_t getH() const;
  uint16_t getL() const;
  uint16_t getF() const;
  
  uint16_t getBC() const;
  uint16_t getHL() const;
  uint16_t getDE() const;
  uint16_t getAF() const;
  uint16_t getSP() const;
  uint16_t getPC() const;
  
  void setA(uint8_t value);
  void setB(uint8_t value);
  void setC(uint8_t value);
  void setD(uint8_t value);
  void setE(uint8_t value);
  void setH(uint8_t value);
  void setL(uint8_t value);
  void setF(uint8_t value);

  void setBC(uint16_t value);
  void setDE(uint16_t value);
  void setHL(uint16_t value);
  void setAF(uint16_t value);

  void setSP(uint16_t pointer);
  void setPC(uint16_t pointer);
  
 private:
  uint8_t A;
  uint8_t B, C;
  uint8_t D, E;
  uint8_t H, L;
  uint8_t F;
  uint16_t SP;
  uint16_t PC;
};
