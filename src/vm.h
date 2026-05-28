#ifndef NANOVM_H
#define NANOVM_H

#include <algorithm>
#include <asmjit/core/codeholder.h>
#include <asmjit/core/cpuinfo.h>
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
#include "error_codes.h"

#define MEM_SIZE 65536
#define STACK_SIZE 4096
#define CALL_STACK_SIZE 2048
#define FETCH (this->memory[this->pc++])

#include "common.h"

using namespace asmjit;


class NowaVM {
    public:
    typedef int (*Func)(void* regs, void* mem, void* stack, void* callstack);

    JitRuntime rt;
    CodeHolder code;
    

    std::array<uint8_t, MEM_SIZE> memory;
    std::array<uint64_t, STACK_SIZE> stack;
    std::array<uint64_t, CALL_STACK_SIZE> callstack;

    // INTERPRETER ONLY 
    uint64_t sp=0;
    uint64_t csp = 0;
    bool zf = false;
    bool less = false;
    bool bigger = false;
    // ----------------

    uint64_t reg[256];
    uint64_t pc=0;
    uint64_t prog_size = 0;
    bool verbose=false;
    bool opt=false;
    bool warning_rt=false;


    NowaVM() {
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


    void run(uint32_t ip);

    int interpret(const uint32_t &ip);

    bool qual_bytecode(uint64_t start, uint64_t end, const uint8_t bytecode[]);


    void analyzer(std::vector<uint8_t> &prog);

    int res();

    void load_program(const std::vector<uint8_t> &prog);

    void register_dump();



    ~NowaVM() {
        
    }
};

#endif