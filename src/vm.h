#ifndef NANOVM_H
#define NANOVM_H

#include <algorithm>
#include <asmjit/core/codeholder.h>
#include <asmjit/core/jitruntime.h>
#include <asmjit/core/operand.h>
#include <asmjit/x86/x86assembler.h>
#include <asmjit/x86/x86operand.h>
#include <asmjit/x86.h>
#include <cstdint>
#include <array>
#include <cstring>
#include <iostream>
#include <vector>

#define MEM_SIZE 65536
#define STACK_SIZE 4096
#define CALL_STACK_SIZE 2048
#define FETCH (this->memory[this->pc++])

#define NUL 0x0
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
#define HLT 0xFF

using namespace asmjit;


class NanoVM {
    public:
    typedef int (*Func)(void* regs, void* mem, void* stack, void* callstack, uint64_t retadr);

    JitRuntime rt;
    CodeHolder code;

    std::array<uint8_t, MEM_SIZE> memory;
    std::array<uint64_t, STACK_SIZE> stack;
    std::array<uint64_t, CALL_STACK_SIZE> callstack;

    uint64_t reg[256];
    uint64_t pc=0;
    uint32_t sp=0;
    uint64_t retaddr=0;
    uint64_t prog_size = 0;

    NanoVM() {
        code.init(rt.environment(), rt.cpu_features());
        for(uint64_t &x : reg) x = 0; 
        for(uint8_t &y : memory) y = 0;
    }


    inline uint64_t fetch64(uint64_t &i) {
        uint64_t res = 0;
        for(int a=0;a<8;a++) {
            res |= (uint64_t)(this->memory[i++]) << (8*a);
        }
        return res;
    }


    void run(int32_t ip) {
        pc = ip;
        int i = 0;
        x86::Assembler a(&code);
        std::vector<Label> labels(prog_size);
        for(auto &x : labels) {
            x = a.new_label();
        }
        
        while(pc<prog_size) {
            a.bind(labels[pc]);
            i=FETCH;
            switch(i) {
                case NUL: continue; break;
                case LD: {
                    uint8_t r = FETCH;
                    uint64_t val = fetch64(pc);
                    
                    a.mov(x86::regs::rax, val);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r * 8), x86::regs::rax);
                    break;
                }
                case ADD: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.add(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case SUB: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.sub(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case MUL: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.mul(x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case DIV: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.cmp(x86::regs::rcx, 0);
                    Label skip_div = a.new_label();
                    a.jz(skip_div);
                    a.xor_(x86::regs::rdx, x86::regs::rdx);
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.div(x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    a.bind(skip_div);
                    break;
                }
                case IMUL: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.imul(x86::regs::rax, x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case IDIV: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.cmp(x86::regs::rcx, 0);
                    Label skip_idiv = a.new_label();
                    a.jz(skip_idiv);
                    a.xor_(x86::regs::rdx, x86::regs::rdx);
                    a.cqo();
                    a.idiv(x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    a.bind(skip_idiv);
                    break;
                }
                case XOR_: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.xor_(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case AND_: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.and_(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case OR_: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.or_(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case SHL: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::cl, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.shl(x86::regs::rax, x86::cl);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case SHR: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::cl, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.shr(x86::regs::rax, x86::cl);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case JMP: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jmp(labels[addr]);
                    break;
                }
                case CMP: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.cmp(x86::regs::rax, x86::regs::rcx);
                    break;
                }
                case JZ: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jz(labels[addr]);
                    break;
                }
                case JNZ: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jnz(labels[addr]);
                    break;
                }
                case JC: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jc(labels[addr]);
                    break;
                }
                case JNC: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jnc(labels[addr]);
                    break;
                }
                case STORE: {
                    uint8_t r = FETCH;
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) {
                        a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                        a.mov(x86::qword_ptr(x86::regs::rsi, addr), x86::regs::rax);
                    }
                    break;
                }
                case LDM: {
                    uint8_t r = FETCH;
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) {
                        a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rsi, addr));
                        a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    }
                    break;
                }
                case JL: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jl(labels[addr]);
                    break;
                }
                case JLE: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jle(labels[addr]);
                    break;
                }
                case JB: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jbe(labels[addr]);
                    break;
                }
                case JBE: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) a.jbe(labels[addr]);
                    break;
                }
                case JMP_REGV: {
                    uint8_t r = FETCH;
                    
                    if(this->reg[r]<prog_size) {
                        a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                        a.jmp(x86::regs::rax);
                    }
                    break;
                }
                case PUSH: {
                    uint8_t r = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::qword_ptr(x86::regs::rdx, this->sp*64), x86::regs::rax);
                    this->sp++;
                    break;
                }
                case POP: {
                    uint8_t r = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdx, this->sp*64));
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    this->sp--;
                    break;
                }
                case HLT: {
                    a.xor_(x86::regs::rax, x86::regs::rax);
                    a.ret();
                    break;
                }
                default: {
                    std::cerr << &"[Error]: Unknown instruction '" << (int)this->memory[pc] << "', stopping execution...\n";
                    pc = 0;
                    return;
                }
            }

        }
    }

    bool qual_bytecode(uint64_t start, uint64_t end,const uint8_t bytecode[]) {
        if(end>=prog_size) return false;
        return std::memcmp(&this->memory+start, bytecode, end-start)==0;
    }


    void analyzer() {
        for(uint64_t i = 0; i<prog_size;) {
            if(this->memory[i]==ADD&&i+5<prog_size&&this->memory[i+3]==ADD) {
                // ADD R0, R1
                // ADD R0, R1
                // -> ADD_SSE R0 R1 R1 R3
                
            }
        }
    }

    int res() {
        Func fn;
        Error err = rt.add(&fn, &code);
        int res = fn(this->reg, &this->memory, &this->stack, &this->callstack, this->retaddr);
        rt.release(fn);
        return res;
    }

    void load_program(const std::vector<uint8_t> &prog) {
        for (uint64_t i = 0; i < prog.size(); i++) {
            if (i >= MEM_SIZE) return;
            memory[i] = prog[i];
        }
        prog_size = prog.size();
    }

    ~NanoVM() {
        
    }
};

#endif