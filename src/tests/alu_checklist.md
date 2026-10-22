// ALU_CHECKLIST.md
// - Regular case without overflow
// - Half-carry (0x0F -> 0x10)
// - Full overflow to zero (0xFF + 1 -> 0x00, Z=1, C=1)
// - Full overlow to non-zero (0xFF + 5 -> 0x04, Z=0, C=1)