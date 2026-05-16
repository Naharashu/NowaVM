

#include <cstdint>
#include <cstring>
#include <vector>
#include "assembly/assembly.h"
#include "vm.h"



int main(int argc, char** argv) {

    NanoVM vm{};
    bool regdump = false;
    std::string filename = "";
    bool runit = false;
    bool compile_only = false;
    bool compiling = false;
    bool opt = false;
    if(argc<2) {
        std::cout << "Usage: nanovm [OPTIONS]\nType nanovm --help for more info\n";
        return 0;
    }
    if(strcmp(argv[1], "-v")==0) {
        std::cout << "NanoVM V0.4\n";
        return 0;
    }
    if(strcmp(argv[1], "--help")==0) {
        if(argc>2&&strcmp(argv[2], "asm")==0) {
            std::cout << "NanoVM have own assembler than turns code like 'LD 0 12' into bytecode '0x01 0x0C 0x00 0x00 ... 0x00.\n";
            std::cout << "-c compiles .asm code of vm into bytecode, this argument often used with -e\n";
            std::cout << "-asm - compiles .asm code of vm into bytecode like -c but it stops after that and prints result as raw numbers\n";
            return 0;
        }
        std::cout << "NanoVM [OPTIONS]:\n";
        std::cout << "-c [filename] - compile .asm file for vm, more details with --help asm\n"
        << "-r - registry dump after execution\n" 
        << "-v - version of NanoVM\n" 
        << "-V - prints generated x64 code\n"
        << "-e - execute bytecode after -c\n"
        << "-asm - only compiles .asm code and prints raw bytecode\n"
        << "-opt - optimize bytecode\n";
    }
    for(int i=1;i<argc;i++) {
        if(strcmp(argv[i], "-r")==0) regdump=true;
        else if(strcmp(argv[i], "-V")==0) vm.verbose=true;
        else if(strcmp(argv[i], "-c")==0&&i+1<argc) {
            filename=argv[i+1];
            i++;
            compiling=true;
        }
        else if(strcmp(argv[i], "-e")==0) runit=true;
        else if(strcmp(argv[i], "-asm")==0) compile_only=true;
        else if(strcmp(argv[i], "-opt")==0) opt=true;
    }
    //vm.load_program(program);
    //vm.run(0);
    //int res = vm.res();
    //std::cout << "Exit code " << res << '\n';

    assembly as;
    try {
        if(compiling) as.init(filename);
    } catch(const assembly_error &e) {
        std::cerr << e.what();
        return 1;
    }
    std::vector<uint8_t> res = as.compile();
    if(compile_only) {
        if(opt) {
            vm.load_program(res);
            vm.analyzer(res);
        }
        for(auto &op : res) {
            std::cout << std::hex <<(int)op << ' ';
        }
        std::cout << std::dec << '\n';
        return 0;
    }

    if(runit) {
        vm.load_program(res);
        vm.opt=opt;
        vm.run(0);
        int exit_code = vm.res();
        return exit_code;
    }

    if(regdump) {
        vm.register_dump();
    }

    return 0;
}