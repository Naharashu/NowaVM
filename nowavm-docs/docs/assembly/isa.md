# NowaVM ISA

---

[bytecode] [opcode] [syntax] [desc]

regX - dest

regY - source

imm - 64 bit integer

addr - absolute VM memory address

l - label

---

### ISA

| bytecode | opcode | syntax | description |
| ---      | ---    | ---    | ---         |
| 0x00     | NOOP   | noop   | does nothing|
| 0x01     | LD   | ld reg imm   | loads imm into register|
| 0x02     | ADD   | add regX regY   | add value of regY to value of regX |
| 0x03     | SUB   | sub regX regY   | sub value of regY from value of regX |
| 0x04     | MUL   | mul regX regY   | mul value of regX by value of regY |
| 0x05     | DIV   | div regX regY   | div value of regX by value of regY |
| 0x06     | IMUL   | imul regX regY   | imul value of regX by value of regY |
| 0x07     | IDIV   | idiv regX regY   | idiv value of regX by value of regY |
| 0x08     | XOR_   | xor regX regY | xores value of regX with value of regY|
| 0x09     | AND_   | and regX regY | logic and between regX and regY |
| 0x0A     | OR_   | or regX regY | logic or between regX and regY |
| 0x0B     | SHL   | shl regX regY  | shifts left value of regX by value of regY |
| 0x0C     | SHR   | shr regX regY   | shifts right value od regX by value of regY |
| 0x0D     | JMP   | jmp l/addr   | jumps to label address or address |
| 0x0E     | CMP   | cmp reg reg   | compares 2 registers |
| 0x0F     | JZ    | jz l/addr  | jumps if zero flag set|
| 0x10     | JNZ   | jnz l/addr   | jumps if zero flag is not set|
| 0x11     | JC   | jc l/addr   | jumps if carry flag is set |
| 0x12     | JNC   | jnc l/addr   | jumps if carry flag is not set |
| 0x13     | STORE   | store reg addr   | stores value of register into memory[addr](8 bits) |
| 0x14     | LDM   | ldm reg addr   | loads value 8 bit value from memory[addr] into register |
| 0x15     | JL   | jl l/addr   | jumps if less flag is set |
| 0x16     | JLE   | jle l/addr   | jumps if less and zero flags are set|
| 0x17     | JB   | jb l/addr   | jumps if bigger flag is set |
| 0x18     | JBE   | jbe l/addr   | jumps id bigger and zero flag is set |
| 0x19     | JMP_REGV   | jmprv reg   | gets value from register and interpretes that as jump address|
| 0x1A     | PUSH   | push reg   | pushes register value into virtual stack |
| 0x1B     | POP   | pop reg   | pops virtual stack top into register |
| 0x1C     | CALL   | call l   | calls funtion and pushes call address into callstack|
| 0x1D     | RET   | ret   | returns to the address that stored at top of callstack|
| 0x1E     | FADD   | fadd regX regY   | adds float value of regY to regX|
| 0x1F     | FSUB   | fsub regX regY   | sub float value of regY from regX|
| 0x20     | FMUL   | fmul regX regY   | mul float value of regY by regX|
| 0x21     | FDIV   | fdiv regX regY   | div float value of regY by regX|
| 0x22     | COPY   | copy regY regX   | copies value of regY to regX|
| 0x23     | SWAP   | swap regX regY   | swaps values of regX and regY|
| 0x24     | FMA   | fma reg reg reg   | fusion-add-mul, e.g. r0=r0*r2+r1||
| 0x25     | LTF   | ltf reg   | converts 64 bit integer into double, e.g. 4->4.0|
| 0x26     | FTL   | ftl reg   | converts double into 64 bit integer, e.g. 4.0->4|
| 0x27     | NOT   | not reg   | inverts bits of register |
| 0x28     | ROR   | ror regX regY   | rotates right value of regX by value of regY|
| 0x29     | ROL   | rol regX regY   | rotates left value of regX by value of regY|
| 0x2A     | ARX   | arx reg reg reg reg   | add-rotateleft-xor, e.g. r0=rotl(r0+r1, r2) xor r3|
| 0x2B     | STORX   | storx regX regY   | stores value of regX into memory[regY*8]|
| 0x2C     | LDMX   | ldmx regX regY     | loads value(8 bit) from memory[regY*8]|
| 0x2D     | LDZERO   | ldzero reg   | loads 0 into register|
| 0xFF     | HLT   | hlt   | ends execution of program |
