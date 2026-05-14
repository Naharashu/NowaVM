
#include <cstdint>
#include <iostream>
#include <vector>
#include "vm.h"



int main(int argc, char** argv) {

    NanoVM vm{};
    std::vector<uint8_t> program = {
        JMP, 20, 0,0,0,0,0,0,0,
        LD, 0, 1,0,0,0,0,0,0, 0,
        RET,
        CALL, 9, 0,0,0,0,0,0,0,
        POP, 0,
        HLT
    };
    vm.load_program(program);
    vm.run(0);
    int res = vm.res();
    std::cout << vm.reg[0] << ' ' << res <<'\n';

    return 0;
}