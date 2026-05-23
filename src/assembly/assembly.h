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
std::unordered_map<std::string, std::string> opcodes = {
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
    explicit lexer(std::string& fname_) : filename(fname_) {
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
    bool use_entry0 = true;
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
        std::cout << finame << '\n';
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
            if(line.rfind("#include", 0)==0) {
                std::string include = line.substr(8);
                trim(include);
                include.erase(0, include.find_first_not_of(" \t"));
                
                res += preprocessor(include);
            } else res += line + '\n';
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
                } else address+=8;
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
                if(id=="include") {
                    while(i<code.size()&&code[i]!='\n') {
                        i++;
                        c++;
                    }
                    continue;
                }
                std::string opcode = opcodes[id];
                if(opcode==""&&!labels.contains(id)) {
                    throw assembly_error("[Error - assembly:" + filename + ':' + std::to_string(l) + ':' + std::to_string(c) + "]: unknown instruction '" + id + "' found in code\n");
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
    std::string file = "";
    void analyze() {
        for(uint64_t i=0;i<lexed.size()-2;i++) {
            token c = lexed[i];
            token n = lexed[i+1];
            token ah = lexed[i+2];
            if(c.t==ID&&n.t==ID&&n.line==c.line) {
                throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) + "]: expected register or immediate after '" + c.val + "', but got '" + n.val + "'\n");
            }
            else if(c.t==INT&&n.t==INT) {
                throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) + "]: expected end of instruction, but got '" + n.val + "'\n");
            } //else if(c.t==ID&&n.t==REGN&&ah.t==ID)
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
        lexer l(filename);
        l.lex();
        lexed = l.lexed;
    }

    std::vector<uint8_t> compile() {
        std::vector<uint8_t> compiled;
        //analyze();
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
                uint64_t val = labels[peek().val];
                consume();
                std::array<uint8_t, 8> bytes = slice64(val);
                for(auto &x : bytes) {
                    compiled.emplace_back(x);
                }
            } else {
                consume();
            }
        }
        return compiled;
    }
};

#endif