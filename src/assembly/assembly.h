#ifndef NANOVM_ASSEMBLY_H
#define NANOVM_ASSEMBLY_H

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include "../common.h"

class assembly_error : public std::exception {
    public:
    std::string err="";
    explicit assembly_error(const std::string &s) {
        err = s;
    }
    const char * what() const noexcept override {
        return err.c_str();
    }
};

enum tok_type : uint8_t
{
    ID,
    COMA,
    SEMI,
    NEWLINE,
    INT,
    SHORT_INT,
    WORD,
    LONG_INT,
    REGN,
    LABEL,
    INCLUDE,
    DEFINE,
    MACRO,
    EMPTY,
    EOF_
};

typedef struct token
{
    tok_type t;
    uint64_t line;
    uint16_t c;
    std::string val = "";
    uint64_t address=0;
} token;

std::unordered_map<std::string, uint64_t> labels;
std::unordered_set<std::string> extern_labels;
std::unordered_map<std::string, std::string> opcodes = {
    {"noop", "0x00"},
    {"ld", "0x01"},
    {"add", "0x02"},
    {"sub", "0x03"},
    {"mul", "0x04"},
    {"div", "0x05"},
    {"imul", "0x06"},
    {"idiv", "0x07"},
    {"xor", "0x08"},
    {"and", "0x09"},
    {"or", "0x0A"},
    {"shl", "0x0B"},
    {"shr", "0x0C"},
    {"jmp", "0x0D"},
    {"cmp", "0x0E"},
    {"jz", "0x0F"},
    {"jnz", "0x10"},
    {"jc", "0x11"},
    {"jnc", "0x12"},
    {"store", "0x13"},
    {"ldm", "0x14"},
    {"jl", "0x15"},
    {"jle", "0x16"},
    {"jb", "0x17"},
    {"jbe", "0x18"},
    {"jmprv", "0x19"},
    {"push", "0x1A"},
    {"pop", "0x1B"},
    {"call", "0x1C"},
    {"ret", "0x1D"},
    {"fadd", "0x1E"},
    {"fsub", "0x1F"},
    {"fmul", "0x20"},
    {"fdiv", "0x21"},
    {"copy", "0x22"},
    {"swap", "0x23"},
    {"fma", "0x24"},
    {"ltf", "0x25"},
    {"ftl", "0x26"},
    {"not", "0x27"},
    {"ror", "0x28"},
    {"rol", "0x29"},
    {"arx", "0x2A"},
    {"storx", "0x2B"},
    {"ldmx", "0x2C"},
    {"ldzero", "0x2D"},
    /*
    {"ADD", "1"},
    {"ADD", "1"},
    {"ADD", "1"},
    {"ADD", "1"},
    {"ADD", "1"},
    */
    {"hlt", "0xFF"},
};

class lexer
{
public:
    std::string code = "";
    std::vector<token> lexed;
    std::string filename = "";
    std::string header;
    bool use_entry0;
    explicit lexer(std::string& fname_, const bool &entr0) : filename(fname_), use_entry0(entr0) {
        code = preprocessor(fname_);
    };

    inline bool is_int(unsigned char c)
    {
        return c >= '0' && c <= '9';
    }

    inline bool is_letter(unsigned char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    inline bool is_opcode(const std::string &id) {
        return (opcodes[id]!="");
    }

    std::unordered_set<std::string> included;
    std::unordered_map<std::string, std::string> defined;
    std::unordered_set<std::string> def_set; 
    std::vector<bool> active_stack; 
    bool active = true; 
    inline std::string preprocessor(const std::string &fname) {
        std::string res;
        std::string line;
        if(use_entry0) {
            std::ifstream entry0(std::string{std::getenv("HOME")}+"/.local/bin/include_nanovm/entry0.asm");
            while(std::getline(entry0, line)) {
                res += line + '\n';
            }
        }
        line = "";
        std::string finame = std::filesystem::path(fname).lexically_normal().string();
        if(included.contains(finame)) return "";
        included.insert(finame);
        std::ifstream f(finame);
        if(!f.is_open()) throw assembly_error("[Error - assembly]: file '" + finame + "' doesnt exitst\n");
        auto trim = [](std::string& s) {
            size_t start = s.find_first_not_of(" \t\r\n");
            size_t end   = s.find_last_not_of(" \t\r\n");

            if (start == std::string::npos) {
                s.clear();
                return;
            }

            s = s.substr(start, end - start + 1);
        };
        while(std::getline(f, line)) {
            if (auto pos = line.find(';'); pos != std::string::npos)
            line = line.substr(0, pos);
            if(line.rfind("#ifdef", 0)==0) {
                std::string name = line.substr(6);
                trim(name);
                active_stack.push_back(active);
                active = active && defined.contains(name);
            } else if(line.rfind("#ifndef", 0)==0) {
                std::string name = line.substr(7);
                trim(name);
                active_stack.push_back(active);
                active = active && !defined.contains(name);
            } else if(line.rfind("#else", 0)==0) {
                if (active_stack.empty()) throw assembly_error("[Error - preprocessor]: unexpected #else\n");
                active = active_stack.back() && !active;
            } else if(line.rfind("#endif", 0)==0) {
                if (active_stack.empty()) throw assembly_error("[Error - preprocessor]: unexpected #endif\n");
                active = active_stack.back();
                active_stack.pop_back();
            } else if(active) {
                if(line.rfind("#include", 0)==0) {
                    std::string include = line.substr(8);
                    trim(include);
                    include.erase(0, include.find_first_not_of(" \t"));
                    
                    res += preprocessor(include);
                } else if(line.rfind("#define", 0)==0) {
                    std::string def = line.substr(7);
                    std::string name;
                    std::string number;
                    bool name_seen=false;
                    bool val_seen=false;
                    for(uint32_t i=0;i<def.size();) {
                        uint8_t c = def[i];
                        if (is_letter(c)||c=='_') {
                            while (i < def.size() && (is_letter(def[i]) || is_int(def[i]) || def[i] == '_')) {
                                name.push_back(def[i]);
                                i++;
                            }
                            name_seen=true;
                        } else if (is_int(c)) {
                            if(!name_seen) {
                                throw assembly_error("[Error - preprocessor]: expected NAME for macro(e.g #define ERROR 1), but got immediate\n");
                            }
                            while ((i < def.size() && is_int(def[i]))) {
                                number.push_back(def[i]);
                                i++;
                            }
                            val_seen=true;
                            defined[name]=number;
                            break;
                        } else i++;
                    }
                    if((!val_seen)&&name_seen) defined[name]="1";
                    else if(!val_seen&&!name_seen) throw assembly_error("[Error - preprocessor]: expected NAME for macro(e.g. #define DEBUG)\n");
                } else if(line.rfind("#undef", 0)==0) {
                    std::string macro_name = line.substr(6);
                    trim(macro_name);
                    if(defined.contains(macro_name)) defined.erase(defined.find(macro_name));
                } else if(line.rfind("#warn", 0)==0) {
                    std::string msg = line.substr(5);
                    trim(msg);
                    msg.erase(0, msg.find_first_not_of(" \t")); 
                    std::cerr << "[Warning]: " << msg << '\n';
                } else if(line.rfind("#error", 0)==0) {
                    std::string msg = line.substr(6);
                    trim(msg);
                    msg.erase(0, msg.find_first_not_of(" \t"));
                    throw assembly_error("[Error - preprocessor]: " + msg + '\n');
                } else if(line.rfind("#extern", 0)==0) {
                    std::string name = line.substr(7);
                    trim(name);
                    name.erase(0, name.find_first_not_of(" \t")); 
                    labels[name] = 0;
                    extern_labels.emplace(name);
                }
                else res += line + '\n';
            }
        }
        return res;
    }

    void collect_labels() {
        uint64_t address = 0;
        for(uint64_t i=0; i < code.size();) {
            uint8_t c = code[i];
            if(is_letter(c)||c=='_') {
                std::string id;
                while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_' || code[i] == ':'))
                {
                    id.push_back(code[i]);
                    i++;
                }
                if(!id.empty() && id.back() == ':') {
                    id.pop_back();
                    labels[id] = address;
                } else if(is_opcode(id)) {
                    address++;
                } else if((id[0]=='R'||id[0]=='r')&&std::all_of(id.begin()+1, id.end(),::isdigit)) {
                    address++;
                } else { 
                    address+=8;
                }
            } else if(is_int(c)) {
                while ((i < code.size() && is_int(code[i])))
                {
                    i++;
                }
                address+=8;
            } else if (c == ';') {
                i++;
                while (i < code.size()&&code[i] != '\n')
                    i++;
            } else i++;
        }
    }

    void lex()
    {

        uint64_t l = 0;
        uint16_t c = 0;
        uint64_t addr=0;
        collect_labels();
        for (uint64_t i = 0; i < code.size();)
        {
            uint8_t s = code[i];
            if (s == ';')
            {
                i++;
                while (i<code.size()&&code[i] != '\n')
                    i++;
            }
            else if (i < code.size()&&s == '\n')
            {
                l++;
                c=0;
                i++;
            }
            else if (s == '\t' || s == '\r' || s == ' ')
            {
                c++;
                i++;
            }
            else if (is_letter(s)||s=='_')
            {
                std::string id;
                while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_' || code[i] == ':'))
                {
                    id.push_back(code[i]);
                    i++;
                    c++;
                }
                if(!id.empty() && id.back() == ':') {
                    id.pop_back();
                    continue;
                }
                if((id[0]=='R'||id[0]=='r')&&std::all_of(id.begin()+1, id.end(),::isdigit)) {
                    std::string_view n(id.data()+1,id.size()-1);
                    lexed.emplace_back(token{REGN, l, c, std::string{n}, addr});
                    addr++;
                    continue;
                }
                if(id=="include"||id=="define") {
                    while(i<code.size()&&code[i]!='\n') {
                        i++;
                        c++;
                    }
                    continue;
                }
                std::string opcode = opcodes[id];
                if(defined.contains(id)) {
                    lexed.emplace_back(token{INT, l, c, defined[id], addr});
                    addr+=8;
                    continue;
                }
                if(opcode==""&&!labels.contains(id)) {
                    throw assembly_error("[Error - assembly:" + filename + ':' + std::to_string(l) + ':' + std::to_string(c) + "]: unknown symbol '" + id + "' found in code\n");
                }
                if(opcode=="") {
                    lexed.emplace_back(token{LABEL, l, c, id, addr});
                    addr+=8;
                    continue;
                }
                lexed.emplace_back(token{ID, l, c, opcode, addr});
                addr++;
            } else if (is_int(s)) {
                std::string number;
                while ((i < code.size() && is_int(code[i])))
                {
                    number.push_back(code[i]);
                    i++;
                    c++;
                }
                lexed.emplace_back(token{INT, l, c, number, addr});
                addr+=8;
            }
            else if (s == ',')
            {
                lexed.emplace_back(token{COMA, l, c, ""});
                c++;
                i++;
            }
            else
            {
                i++;
                c++;
            }
        }
        lexed.emplace_back(token{EOF_, l, c, "", addr});
    }
};

class assembly {
    private:
    std::vector<token> lexed;
    std::array<uint8_t, 8> slice64(uint64_t i) {
        std::array<uint8_t, 8> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }
    std::array<uint8_t, 4> slice32(uint32_t i) {
        std::array<uint8_t, 4> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }
    std::array<uint8_t, 2> slice16(uint16_t i) {
        std::array<uint8_t, 2> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }
    uint64_t indx=0;
    void consume() {
        indx++;
    }
    token& peek() {
        if(indx<lexed.size()) return lexed[indx];
        else return lexed.back();
    }
    public:
    bool opt=false;
    bool use_entry0_as=true;
    std::string file = "";
    void analyze() {
        if(!opt) return;
        std::array<uint8_t, 256> register_usage{};
        for(auto &x : lexed) {
            if(x.t==REGN) register_usage[std::stoull(x.val)%256]++;
        }
        uint64_t s = lexed.size()-2;
        for(uint64_t i=0;i<s;i++) {
            token c = lexed[i];
            token n = lexed[i+1];
            token ah = lexed[i+2];
            if(c.t==ID&&n.t==ID&&n.line==c.line) {
                throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) + "]: expected register or immediate after '" + c.val + "', but got '" + n.val + "'\n");
            }
            else if(c.t==INT&&n.t==INT) {
                throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) + "]: expected end of instruction, but got '" + n.val + "'\n");
            } //else if(c.t==ID&&n.t==REGN&&ah.t==ID) 
            else if(c.val=="0xFF"&&n.t!=EOF_) {
                lexed[i+1]={EOF_};
                break;
            } else if(c.val=="0x0D"&&n.t==LABEL) {
                // jmp label 0 1..8
                // label:
                // ->
                // noop (just executes linearly)
                auto x = labels.find(n.val);
                if(x != labels.end() && ah.address == x->second) {
                    lexed[i] = {EMPTY};
                    lexed[i+1] = {EMPTY};
                }
            } else if(c.val=="0x01"&&(n.t==REGN&&register_usage[std::stoull(n.val)]==1)) {
                // ld r0 42
                // ->
                // noop (never used)
                lexed[i] = {EMPTY};
                lexed[i+1] = {EMPTY};
                lexed[i+2] = {EMPTY};
            } else if((c.val=="0x08"||c.val=="0x03")&&(n.t==REGN&&ah.t==REGN)&&n.val==ah.val) {
                lexed[i].val = "0x2D";
                std::cout << lexed[i].val << ' ';
                lexed[i+2] = {EMPTY};
            }
            i+=2;
        }
    }

    void init(std::string &filename) {
        file = filename;
        std::ifstream f(filename);
        if(!f.is_open()) throw assembly_error("[Error - assembly]: file '" + filename + "' doesnt exitst\n");
        std::string line;
        std::string code;
        while(std::getline(f, line)) {
            code += line + '\n';
        }
        lexer l(filename, use_entry0_as);
        l.lex();
        lexed = l.lexed;
    }

    std::vector<uint8_t> compile() {
        std::string linker_sym_name=file.substr(0,file.size()-3)+"sym";
        std::ofstream f(linker_sym_name);
        std::vector<uint8_t> compiled;
        analyze();
        while(indx<lexed.size()&&peek().t!=EOF_) {
            if(lexed[indx].t==ID) {
                std::string id = lexed[indx].val;
                compiled.emplace_back(std::stoul(id, 0, 16));
                consume();
                continue;
            } else if(peek().t==INT) {
                uint64_t val = std::stoull(lexed[indx].val);
                consume();
                std::array<uint8_t, 8> bytes = slice64(val);
                for(auto &x : bytes) {
                    compiled.emplace_back(x);
                }
            } else if(peek().t==REGN) {
                compiled.emplace_back(std::stol(lexed[indx].val)%256);
                consume();
            } else if(peek().t==LABEL) {
                std::string id = peek().val;
                f << "RELOC " << compiled.size() << ' ' << id << '\n';
                uint64_t val = labels[id];
                consume();
                std::array<uint8_t, 8> bytes = slice64(val);
                for(auto &x : bytes) {
                    compiled.emplace_back(x);
                }
            } else if(peek().t==MACRO) {
                uint64_t val = stoull(peek().val);
                consume();
                std::array<uint8_t, 8> bytes = slice64(val);
                for(auto &x : bytes) {
                    compiled.emplace_back(x);
                }
            } else {
                consume();
            }
        }
        for(auto &x : labels) {
            if(!extern_labels.contains(x.first)) f << "EXPORT " << x.first << ' ' << x.second << '\n';
        }
        f.close();
        return compiled;
    }
};

#endif