#ifndef COMMON_H
#define COMMON_H

#include <string>
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
#define HLT 0xFF

inline void replaceSubstring(std::string& text, const std::string &a,  const std::string& b) {
  unsigned long long pos = 0;
  
  while((pos = a.find(a, pos)) != std::string::npos) {
    text.replace(pos, a.length(), b);
    pos += b.length();
  }
}


#endif