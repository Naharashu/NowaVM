#ifndef NANOVM_ASSEMBLY_H
#define NANOVM_ASSEMBLY_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <string>
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


class lexer
{
public:
    std::string code = "";
    std::vector<token> lexed;
    explicit lexer(const std::string &c) : code(c) {};

    inline bool is_int(unsigned char c)
    {
        return c >= '0' && c <= '9';
    }

    inline bool is_letter(unsigned char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    void lex()
    {
        uint64_t l = 0;
        uint16_t c = 0;
        uint64_t addr=0;
        for (uint64_t i = 0; i < code.size();)
        {
            uint8_t s = code[i];
            if (s == ';')
            {
                i++;
                while (code[i] != '\n')
                    i++;
            }
            else if (s == '\n')
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
            else if (is_letter(s))
            {
                std::string id;
                while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_'))
                {
                    id.push_back(code[i]);
                    i++;
                    c++;
                }
                addr++;
                std::string opcode = "0";
                if(id=="ld") opcode = "0x01";
                if(id=="add") opcode = "0x02";
                if(id=="sub") opcode = "0x03";
                if(id=="mul") opcode = "0x04";
                if(id=="div") opcode = "0x05";
                if(id=="imul") opcode = "0x06";
                if(id=="idiv") opcode = "0x07";
                if(id=="xor") opcode = "0x08";
                if(id=="and") opcode = "0x09";
                if(id=="or") opcode = "0x0A";
                if(id=="shl") opcode = "0x0B";
                if(id=="shr") opcode = "0x0C";
                if(id=="jmp") opcode = "0x0D";
                if(id=="cmp") opcode = "0x0E";
                if(id=="jz") opcode = "0x0F";
                if(id=="jnz") opcode = "0x10";
                if(id=="jc") opcode = "0x11";
                if(id=="jnc") opcode = "0x12";
                if(id=="store") opcode = "0x13";
                if(id=="ldm") opcode = "0x14";
                if(id=="jl") opcode = "0x15";
                if(id=="jle") opcode = "0x16";
                if(id=="jb") opcode = "0x17";
                if(id=="jbe") opcode = "0x18";
                if(id=="jmprv") opcode = "0x19";
                if(id=="push") opcode = "0x1A";
                if(id=="pop") opcode = "0x1B";
                if(id=="call") opcode = "0x1C";
                if(id=="ret") opcode = "0x1D";
                if(id=="fadd") opcode = "0x1E";
                if(id=="fsub") opcode = "0x1F";
                if(id=="fmul") opcode = "0x20";
                if(id=="fdiv") opcode = "0x21";
                /*
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                */
                if(id=="hlt") opcode = "0xFF";
                lexed.push_back(token{ID, l, c, opcode, addr});
            }
            else if (is_int(s))
            {
                std::string number;
                while ((i < code.size() && is_int(code[i])))
                {
                    number.push_back(code[i]);
                    i++;
                    c++;
                }
                char* endptr;
                uint64_t val = strtoull(number.c_str(), &endptr, 0);
                if(val <= UINT8_MAX) {
                    addr++;
                    lexed.push_back(token{INT, l, c, number, addr});
                }
                //else if(val <= UINT16_MAX) lexed.push_back(token{SHORT_INT, l, c, number});
                //else if(val <= UINT32_MAX) lexed.push_back(token{WORD, l, c, number});
                else {
                    addr+=8;
                    lexed.push_back(token{LONG_INT, l, c, number, addr});
                }
            }
            else if (s == ',')
            {
                lexed.push_back(token{COMA, l, c, ""});
                c++;
                i++;
            }
            else
            {
                i++;
                c++;
            }
        }
        lexed.push_back(token{EOF_, l, c, "", addr});
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
    token peek() {
        if(indx<lexed.size()) return lexed[indx];
        else return token{EOF_, 0,0,""};
    }
    public:
    void init(const std::string &filename) {
        std::ifstream f(filename);
        if(!f.is_open()) throw assembly_error("[Error - assembly]: file '" + filename + "' doesnt exitst\n");
        std::string line;
        std::string code;
        while(std::getline(f, line)) {
            code += line + '\n';
        }
        lexer l(code);
        l.lex();
        lexed = l.lexed;
    }

    std::vector<uint8_t> compile() {
        std::vector<uint8_t> compiled;
        while(indx<lexed.size()&&peek().t!=EOF_) {
            if(lexed[indx].t==ID) {
                std::string id = lexed[indx].val;
                compiled.push_back(std::stoul(id, 0, 16));
                consume();
                if(peek().t==INT) {
                    if(id=="0x01") {
                        compiled.push_back(std::stoul(lexed[indx].val));
                        compiled.push_back(0);
                        compiled.push_back(0);
                        compiled.push_back(0);
                        compiled.push_back(0);
                        compiled.push_back(0);
                        compiled.push_back(0);
                        compiled.push_back(0);
                    }
                    else {
                        compiled.push_back(std::stoul(lexed[indx].val)%256);
                    }
                    consume();
                } else if (peek().t==LONG_INT) {
                    uint64_t val = std::stoull(lexed[indx].val);
                    consume();
                    std::array<uint8_t, 8> bytes = slice64(val);
                    for(auto &x : bytes) {
                        compiled.push_back(x);
                    }
                }
            } else if(peek().t==INT) {
                compiled.push_back(std::stoul(lexed[indx].val)%256);
                consume();
            } else {
                consume();
            }
        }
        return compiled;
    }
};

#endif