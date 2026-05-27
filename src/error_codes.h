#ifndef NANOVM_ERR_CODES_H
#define NANOVM_ERR_CODES_H

#include <cstdint>
#include <string>
#define UNKNOWN_ERROR 1
#define STACK_OVERFLOW 2
#define STACK_UNDERFLOW 3
#define NO_RETURN_ADDRESS 4
#define OUT_OF_BOUND_MEMORY_ACCESS_READ 5
#define OUT_OF_BOUND_MEMORY_ACCESS_WRITE 6
#define CANNOT_INTERPRET_THIS_OPCODE 7
#define CALL_STACK_OVERFLOW 8
#define OUT_OF_BOUND_JUMP 9
#define INTERPRETER_ONLY_OPCODE 10

inline std::string exit_code_info(uint8_t i) {
    switch (i) {
        case 0: return "SUCCESS";
        case 1: return "UNKNOWN_ERROR";
        case 2: return "STACK_OVERFLOW";
        case 3: return "STACK_UNDERFLOW";
        case 4: return "NO_RETURN_ADDRESS";
        case 5: return "OUT_OF_BOUND_MEMORY_ACCESS_READ";
        case 6: return "OUT_OF_BOUND_MEMORY_ACCESS_WRITE";
        case 7: return "CANNOT_INTERPRET_THIS_OPCODE";
        case 8: return "CALL_STACK_OVERFLOW";
        case 9: return "OUT_OF_BOUND_JUMP";
        case 10: return "INTERPRETER_ONLY_OPCODE";
        default: return "UNKNOWN_ERROR";
    }
}

#endif
