#include <cstdint>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <vector>
#include "assembly/assembly.h"
#include "error_codes.h"
#include "vm.h"



int main(int argc, char** argv) {
    bool regdump = false;
    std::string filename = "";
    bool runit = false;
    bool runit_file = false;
    std::string filename_bytecode="";
    bool compiling = false;
    std::string output_file_name="out.bin";
    bool opt = false;
    bool entry0 = true;
    #ifndef __aarch64__
    bool interpret_this=false;
    #else
    bool interpret_this=true;
    #endif
    if(argc<2) {
        std::cout << "Usage: nanovm [OPTIONS]\nType nanovm --help for more info\n";
        return 0;
    }
    if(strcmp(argv[1], "-v")==0) {
        std::cout << "NanoVM V0.7\n";
        return 0;
    }
    if(strcmp(argv[1], "--help")==0) {
        if(argc>2&&strcmp(argv[2], "asm")==0) {
            std::cout << "NanoVM have own assembler than turns code like 'LD 0 12' into bytecode '0x01 0x0C 0x00 0x00 ... 0x00.\n";
            std::cout << "-c compiles .asm code of vm into bytecode and writes result into binary file, you can run immetiatly with -e\n";
            std::cout << "-o - sets name for output binary file name, by default name is 'out.bin'\n";
            std::cout << "-fno-entry0 - does not injects entry0.asm(jmp _start) into main file\n";
            return 0;
        }
        std::cout << "NanoVM [OPTIONS] <input file>:\n"
        << "-r - registry dump after execution\n" 
        << "-v - version of NanoVM\n" 
        << "-V - prints generated x64 code\n"
        << "-Wrt - prints VM runtime warnings\n"
        << "-e [filename] - execute bytecode\n"
        << "-o - sets name for output binary file\n"
        << "-opt - optimize bytecode\n"
        << "-fno-entry0 - not include entry0.asm\n"
        << "-interpret - use interpreter for execution\n";
        return 0;
    }
    NanoVM vm{};
    for(int i=1;i<argc;i++) {
        if(strcmp(argv[i], "-r")==0) regdump=true;
        else if(strcmp(argv[i], "-V")==0) vm.verbose=true;
        else if(strcmp(argv[i], "-Wrt")==0) vm.warning_rt=true;
        else if(strcmp(argv[i], "-e")==0) {
            if(i+1<argc&&argv[i+1][0]!='-'&&!compiling) {
                runit_file=true;
                filename_bytecode=argv[i+1];
                i++;
                continue;
            }
            else runit=true;
        }
        else if(strcmp(argv[i], "-opt")==0) opt=true;
        else if(strcmp(argv[i], "-o")==0) {
            if(i+1<argc) output_file_name=argv[i+1];
            else {
                std::cerr << "[Error]: Output file name not specified(example: -o out.bin)\n";
                return 1;
            }
            i++;
        }
        else if(strcmp(argv[i], "-fno-entry0")==0) {
            entry0 = false;
        }
        else if(strcmp(argv[i], "-interpret")==0) {
            interpret_this = true;
        }
        else if(!compiling&&argv[i][0]!='-') {
            filename=argv[i];
            compiling=true;
        }
        else {
            std::cerr << "[Error]: Passed unknown argument '" << argv[i] << "'\n";
            return 1;
        }
    }
    //vm.load_program(program);
    //vm.run(0);
    //int res = vm.res();
    //std::cout << "Exit code " << res << '\n';

    assembly as;
    as.opt=opt;
    as.use_entry0_as=entry0;
    try {
        if(compiling) as.init(filename);
    } catch(const assembly_error &e) {
        std::cerr << e.what();
        return 1;
    }
    std::vector<uint8_t> res;
    try {
        res = as.compile();
    } catch(const assembly_error &e) {
        std::cerr << e.what();
        return 1;
    }
    if(compiling) {
        if(opt) {
            vm.analyzer(res);
        }
        
        std::ofstream f(output_file_name, std::ios::out | std::ios::binary);
        f.write(reinterpret_cast<const char*>(res.data()), res.size()*sizeof(res[0]));
        f.close();
    }

    if(runit) {
        vm.load_program(res);
        vm.opt=opt;
        int exit_code=0;
        if(!interpret_this) {
            vm.run(0);
            exit_code = vm.res();
        } else {
            exit_code = vm.interpret(0);
        }
        std::cout << "Program exited with code " << exit_code << '(' << exit_code_info(exit_code) << ")\n";
    }

    if(runit_file) {
        std::fstream f(filename_bytecode, std::ios::in | std::ios::out | std::ios::binary);
        if(!f.is_open()) {
            std::cerr << "[Error]: file '" + filename_bytecode + "' doesnt exist!\n";
            return 1;
        }
        std::vector<uint8_t> bytecode;
        uint8_t c;
        while(f.read(reinterpret_cast<char*>(&c), sizeof(c))) {
            bytecode.emplace_back(c);
        }
        vm.load_program(bytecode);
        int exit_code=0;
        if(!interpret_this) {
            vm.run(0);
            exit_code = vm.res();
        } else {
            exit_code = vm.interpret(0);
        }
        std::cout << "Program exited with code " << exit_code << '(' << exit_code_info(exit_code) << ")\n";
    }

    if(regdump) {
        vm.register_dump();
    }

    return 0;
}