#ifndef COMMON_H
#define COMMON_H

#include <string>

// Basic Pipiline
#define NOOP 0x0
#define LD 0x01
#define ADD 0x02 
#define SUB 0x03
#define MUL 0x04
#define DIV 0x05
#define IMUL 0x06
#define IDIV 0x07
#define XOR_ 0x08
#define AND_ 0x09
#define OR_ 0x0A
#define SHL 0x0B
#define SHR 0x0C
#define JMP 0x0D
#define CMP 0x0E
#define JZ 0x0F
#define JNZ 0x10
#define JC 0x11
#define JNC 0x12
#define STORE 0x13 
#define LDM 0x14
#define JL 0x15
#define JLE 0x16
#define JB 0x17
#define JBE 0x18
#define JMP_REGV 0x19
#define PUSH 0x1A
#define POP 0x1B
#define CALL 0x1C
#define RET 0x1D
#define FADD 0x1E
#define FSUB 0x1F
#define FMUL 0x20
#define FDIV 0x21
#define COPY 0x22
#define SWAP 0x23
#define FMA 0x24
#define LTF 0x25
#define FTL 0x26
#define NOT 0x27
#define ROR 0x28
#define ROL 0x29
#define ARX 0x2A
#define STORX 0x2B
#define LDMX 0x2C
#define LDZERO 0x2D
#define PRINT_REG 0x2E
#define INPUT_REG 0x2F
#define NEG 0x30
#define INC 0x31
#define DEC 0x32

// SIMD 128 bit ( NowaVM VEXT-1)

#define VADDQW 0x33
#define VSUBQW 0x34
#define VDIVQW 0x35
#define VMULQW 0x36
#define VCMP 0x37
#define VSHLQW 0x38
#define VSHRQW 0x39
#define VXOR 0x3A
#define VAND 0x3B
#define VOR 0x3C
#define VSWAP 0x3E
#define VCOPY 0x3D
#define VLDQW 0x3F
#define VLDDW 0x40
#define VLDW 0x41
#define VLD 0x42
#define VLDSP 0x43
#define VLDDP 0x44
#define VLDMXQW 0x45
#define VLDLQW 0x46 // VLDQW + line of QW(0 or 1)
#define VNOT 0x47
#define VADDDW 0x48
#define VSUBDW 0x49
#define VDIVDW 0x4A
#define VMULDW 0x4B
#define VADDW 0x4C
#define VSUBW 0x4D
#define VDIVW 0x4E
#define VMULW 0x4F
#define VADD 0x50
#define VSUB 0x51
#define VDIV 0x52
#define VMUL 0x53
#define VADDSP 0x54
#define VSUBSP 0x55
#define VDIVSP 0x56
#define VMULSP 0x57
#define VADDDP 0x58
#define VSUBDP 0x59
#define VDIVDP 0x5A
#define VMULDP 0x5B
#define VSTREGQW 0x5C

#define VEXT_2_POPCNT 0x01
#define VEXT_2_CLZ 0x02
#define VEXT_2_ARX 0x03
#define VEXT_2_LDM 0x04

#define EXTENSION 0xFE

#define HLT 0xFF

inline void replaceSubstring(std::string& text, const std::string &a,  const std::string& b) {
  unsigned long long pos = 0;
  
  while((pos = a.find(a, pos)) != std::string::npos) {
    text.replace(pos, a.length(), b);
    pos += b.length();
  }
}


#endif