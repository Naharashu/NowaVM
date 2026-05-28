# NowaVM ISA

---

[bytecode] [opcode] [syntax] [desc]

regX - регістр признечення

regY - регістр джерела

imm - 64 бітне число

addr - абсолютна адреса пам'яті ВМ

l - мітка

---

### ISA

| байткод | назва опкоду | синтаксис | опис |
| ---      | ---    | ---    | ---         |
| 0x00     | NOOP   | noop   | нічого не робить |
| 0x01     | LD   | ld reg imm   | завантажити число у регістр |
| 0x02     | ADD   | add regX regY   | додати regY до regX |
| 0x03     | SUB   | sub regX regY   | відняти regY від regX |
| 0x04     | MUL   | mul regX regY   | помножити regX на regY |
| 0x05     | DIV   | div regX regY   | поділити regX на regY |
| 0x06     | IMUL   | imul regX regY   | знакове множення regX на regY |
| 0x07     | IDIV   | idiv regX regY   | знакове ділення regX на regY |
| 0x08     | XOR_   | xor regX regY | виключне або між regX та regY|
| 0x09     | AND_   | and regX regY | логічне та між regX та regY |
| 0x0A     | OR_   | or regX regY | логічне або між regX та regY |
| 0x0B     | SHL   | shl regX regY  | зсунути вліво значення regX на значення regY |
| 0x0C     | SHR   | shr regX regY   | зсунути вправо regX на значення regY |
| 0x0D     | JMP   | jmp l/addr   | перейти до адреси або мітки |
| 0x0E     | CMP   | cmp reg reg   | порівняти два регістри |
| 0x0F     | JZ    | jz l/addr  | стрибнути якщо нульовий прапорець |
| 0x10     | JNZ   | jnz l/addr   | стрибнути якщо не нульовий прапорець|
| 0x11     | JC   | jc l/addr   | стрибнути якщо carry прапорець |
| 0x12     | JNC   | jnc l/addr   | стрибнути якщо не carry прапорець |
| 0x13     | STORE   | store reg addr   | зберігає 8 бітне значення регістру у memory[addr](8 бітів) |
| 0x14     | LDM   | ldm reg addr   | завантажує 8 бітне значення з memory[addr] в регістр |
| 0x15     | JL   | jl l/addr   | стрибнути якщо менше |
| 0x16     | JLE   | jle l/addr   | стрибнути якщо менше та нульовий прапорець|
| 0x17     | JB   | jb l/addr   | стрибнути якщо більше |
| 0x18     | JBE   | jbe l/addr   | стрибнути якщо більше та нульовий прапорець |
| 0x19     | JMP_REGV   | jmprv reg   | (тільки в інтерпретаторі) бере значення з регістра та інтерпретує її як адресу для переходу|
| 0x1A     | PUSH   | push reg   | зберігає значення регістра на вершину стеку |
| 0x1B     | POP   | pop reg   | зберігає значення з вершини стеку у регістр та зменшує sp на 1 |
| 0x1C     | CALL   | call l   | викликає функцію та зберігає адресу на callstack|
| 0x1D     | RET   | ret   | повертається до адреси збереженої на вершині callstack|
| 0x1E     | FADD   | fadd regX regY   | додавання з плавучою комою regY до regX|
| 0x1F     | FSUB   | fsub regX regY   | віднімання з плавучою комою regY від regX|
| 0x20     | FMUL   | fmul regX regY   | множення з плавучою комою regY на regX|
| 0x21     | FDIV   | fdiv regX regY   | ділення з плавучою комою regY на regX|
| 0x22     | COPY   | copy regY regX   | копіює значення regY до regX|
| 0x23     | SWAP   | swap regX regY   | міняє місцями значення regX та regY|
| 0x24     | FMA   | fma reg reg reg   | fusion-add-mul, наприклад. r0=r0*r2+r1||
| 0x25     | LTF   | ltf reg   | завантажує 64 бітне число та перетворює його у число з плавучою комою, наприклад 4->4.0|
| 0x26     | FTL   | ftl reg | конвертує значення з плавучою комою назад у 64 бітне число, наприклад 4.0->4|
| 0x27     | NOT   | not reg   | інвертує значення регістра побітово |
| 0x28     | ROR   | ror regX regY   | прокручує вправо значення regX на значення regY|
| 0x29     | ROL   | rol regX regY   | прокручує вліво значення regX на значення regY|
| 0x2A     | ARX   | arx reg reg reg reg   | add-rotateleft-xor, приклад r0=rotl(r0+r1, r2) xor r3|
| 0x2B     | STORX   | storx regX regY   | зберігає нижні 8 бітів значення regX в  memory[regY*8]|
| 0x2C     | LDMX   | ldmx regX regY     | завантажує 8 бітне значення з memory[regY*8]|
| 0x2D     | LDZERO   | ldzero reg   | завантажує 0 у регістр |
| 0xFF     | HLT   | hlt   | закінчує виконання програми |
