#ifndef NANOVM_H
#define NANOVM_H

#include <algorithm>
#include <asmjit/core/codeholder.h>
#include <asmjit/core/jitruntime.h>
#include <asmjit/core/logger.h>
#include <asmjit/core/operand.h>
#include <asmjit/x86/x86assembler.h>
#include <asmjit/x86/x86operand.h>
#include <asmjit/x86.h>
#include <bit>
#include <cstdint>
#include <array>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#define MEM_SIZE 65536
#define STACK_SIZE 4096
#define CALL_STACK_SIZE 2048
#define FETCH (this->memory[this->pc++])

#include "common.h"

using namespace asmjit;


class NanoVM {
    public:
    typedef int (*Func)(void* regs, void* mem, void* stack, void* callstack);

    JitRuntime rt;
    CodeHolder code;
    

    std::array<uint8_t, MEM_SIZE> memory;
    std::array<uint64_t, STACK_SIZE> stack;
    std::array<uint64_t, CALL_STACK_SIZE> callstack;

    uint64_t reg[256];
    uint64_t pc=0;
    uint64_t prog_size = 0;
    bool verbose=false;
    bool opt=false;

    NanoVM() {
        code.init(rt.environment(), rt.cpu_features()); 
        for(uint64_t &x : reg) x = 0; 
        for(uint8_t &y : memory) y = 0;
    }

    std::array<uint8_t, 8> slice64(uint64_t i) {
        std::array<uint8_t, 8> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }

    inline uint64_t fetch64(uint64_t &i) {
        uint64_t res = 0;
        for(int a=0;a<8;a++) {
            res |= (uint64_t)(this->memory[i++]) << (8*a);
        }
        return res;
    }

    inline uint64_t into64(uint64_t &i, std::vector<uint8_t> m) {
        uint64_t res = 0;
        for(int a=0;a<8;a++) {
            res |= (uint64_t)(m[i+a]) << (8*a);
        }
        return res;
    }

    

    void run(int32_t ip) {
        StringLogger logger;
        code.set_logger(&logger);
        pc = ip;
        int i = 0;
        x86::Assembler a(&code);
        std::vector<Label> labels(prog_size);
        for(auto &x : labels) {
            x = a.new_label();
        }
        
        a.mov(x86::r10, x86::rcx); 
        a.xor_(x86::r12, x86::r12); 

        a.xor_(x86::r11, x86::r11); 
        a.xor_(x86::regs::rcx, x86::regs::rcx);

        a.push(x86::regs::r12);
        a.push(x86::regs::r15);
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
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.mov(x86::cl, x86::regs::rcx);
                    a.shl(x86::regs::rax, x86::cl);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case SHR: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.mov(x86::cl, x86::regs::rcx);
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
                    uint64_t addr = this->reg[r];
                    
                    if(addr<prog_size) {
                        a.jmp(labels[addr]);
                    }
                    break;
                }
                case PUSH: {
                    uint8_t r = FETCH;
                    Label skip_ = a.new_named_label("Stack_PUSH");
                    a.cmp(x86::regs::r11, STACK_SIZE);
                    a.jb(skip_);
                    a.pop(x86::regs::r15);
                    a.pop(x86::regs::r12);
                    a.mov(x86::regs::rax, 1);
                    a.ret();
                    a.bind(skip_);
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::qword_ptr(x86::regs::rdx, x86::regs::r11, 3), x86::regs::rax);
                    a.inc(x86::regs::r11);
                    break;
                }
                case POP: {
                    uint8_t r = FETCH;
                    Label skip_ = a.new_named_label("Stack_POP");
                    a.cmp(x86::regs::r11, 0);
                    a.jnz(skip_);
                    a.pop(x86::regs::r15);
                    a.pop(x86::regs::r12);
                    a.mov(x86::regs::rax, 1);
                    a.ret();
                    a.bind(skip_);
                    a.dec(x86::regs::r11);
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdx, x86::regs::r11, 3));
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    
                    break;
                }
                case CALL: {
                    uint64_t addr = fetch64(pc);
                    
                    if(addr<prog_size) {
                        Label ret_label = a.new_label();
                        a.cmp(x86::regs::r12, CALL_STACK_SIZE);
                        a.jae(ret_label);
                        a.lea(x86::regs::r15, x86::ptr(ret_label));
                        a.mov(x86::qword_ptr(x86::regs::r10, x86::regs::r12, 3), x86::regs::r15);
                        a.inc(x86::regs::r12);
                        a.jmp(labels[addr]);
                        a.bind(ret_label);
                    }
                    break;
                }
                case RET: {
                    a.cmp(x86::regs::r12, 0);
                    Label skip_ = a.new_label();
                    a.jz(skip_);
                    a.dec(x86::regs::r12);
                    a.mov(x86::regs::r15,x86::qword_ptr(x86::regs::r10, x86::regs::r12, 3));
                    a.jmp(x86::regs::r15);
                    a.bind(skip_);
                    break;
                }
                case FADD: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.movq(x86::regs::xmm0, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.movq(x86::regs::xmm1, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.addsd(x86::regs::xmm0, x86::regs::xmm1);
                    a.movq(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::xmm0);
                    break;
                }
                case FSUB: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.movq(x86::regs::xmm0, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.movq(x86::regs::xmm1, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.subsd(x86::regs::xmm0, x86::regs::xmm1);
                    a.movq(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::xmm0);
                    break;
                }
                case FMUL: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.movq(x86::regs::xmm0, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.movq(x86::regs::xmm1, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.mulsd(x86::regs::xmm0, x86::regs::xmm1);
                    a.movq(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::xmm0);
                    break;
                }
                case FDIV: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.movq(x86::regs::xmm1, x86::qword_ptr(x86::regs::rdi, r2*8));
                    Label skip_div = a.new_named_label("skip_div_by_zero");
                    a.xorpd(x86::regs::xmm0, x86::regs::xmm0);
                    a.ucomisd(x86::regs::xmm1, x86::regs::xmm0);
                    a.je(skip_div);
                    a.movq(x86::regs::xmm0, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.divsd(x86::regs::xmm0, x86::regs::xmm1);
                    a.movq(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::xmm0);
                    a.bind(skip_div);
                    break;
                }
                case HLT: {
                    a.pop(x86::regs::r15);
                    a.pop(x86::regs::r12);
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
        if(this->verbose) std::cout << logger.data();
    }

    bool qual_bytecode(uint64_t start, uint64_t end,const uint8_t bytecode[]) {
        if(end>=prog_size) return false;
        return std::memcmp(this->memory.data()+start, bytecode, end-start)==0;
    }


    void analyzer(std::vector<uint8_t> &prog) {
        for(uint64_t i = 0; i<prog.size();) {
            if(prog[i]==LD&&i+23<prog.size()&&prog[i+10]==LD&&(prog[i+20]==ADD||prog[i+20]==SUB||prog[i+20]==MUL||prog[i+20]==DIV||prog[i+20]==IMUL||prog[i+20]==IDIV)) {
                // LD R0, imm1(8)
                // LD R1, imm2(8)
                // ADD, R0, R1
                // ->
                // LD, R0, imm1+imm2
                // LD, R1, imm2

                std::cout << "started constan folding\n";
                uint8_t a = prog[i+1];
                uint8_t b = prog[i+11];
                uint8_t c = prog[i+21];
                uint8_t d = prog[i+22];
                if(a!=c||b!=d) {
                    i+=23;
                    continue;
                }
                uint64_t temp_indx1 = i+2;
                uint64_t temp_indx2 = i+12;
                uint64_t val = into64(temp_indx1, prog);
                uint64_t val2 = into64(temp_indx2, prog);
                uint8_t opcode = prog[i+20];
                uint64_t v = 0;
                switch(opcode) {
                    case ADD: v = val+val2; break;
                    case SUB: v = val-val2; break;
                    case MUL: v = val*val2; break;
                    case DIV: v = val/val2; break;
                    case IMUL: v = (int64_t)val*(int64_t)val2; break;
                    case IDIV: v = (int64_t)val/(int64_t)val2; break;
                }

                std::array<uint8_t, 8> newvalr0 = slice64(v);
                temp_indx1=0;
                for(uint64_t j=i+2;j<i+10;j++) {
                    prog[j] = newvalr0[temp_indx1]; 
                    temp_indx1++;
                }
                for(uint64_t k=i+20;k<i+23;k++) prog[k]=0x00; // noop
                i+=23;
            } else i++;
        }
    }

    int res() {
        Func fn;
        Error err = rt.add(&fn, &code);
        int res = fn(this->reg, this->memory.data(), this->stack.data(), this->callstack.data());
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

    void register_dump() {
        std::cout << "NanoVM register dump(Zero-value registers not printed):\n"; 
        for(uint16_t i=0;i<256;i++) {
            auto val = this->reg[i];
            if(val!=0) {
                std::cout << "R" << i << " = " << val << ' ';
                if(i!=0&&(i%8)==0) std::cout << '\n';
            }
        }
        std::cout << '\n';
        return;
    }

    

    ~NanoVM() {
        
    }
};

#endif