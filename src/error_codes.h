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

inline std::string exit_code_info(uint8_t i) {
    switch (i) {
        case 0: return "SUCCESS";
        case 1: return "UNKNOWN_ERROR";
        case 2: return "STACK_OVERFLOW";
        case 3: return "STACK_UNDERFLOW";
        case 4: return "NO_RETURN_ADDRESS";
        case 5: return "OUT_OF_BOUND_MEMORY_ACCESS_READ";
        case 6: return "OUT_OF_BOUND_MEMORY_ACCESS_WRITE";
    }
}

#endif
