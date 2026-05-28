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

    bool is_opcode(const std::string &id);

    std::unordered_set<std::string> included;
    std::unordered_map<std::string, std::string> defined;
    std::unordered_set<std::string> def_set; 
    std::vector<bool> active_stack; 
    bool active = true;
    std::string preprocessor(const std::string &fname);

    void collect_labels();

    void lex();
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
        else {
            throw assembly_error("[Error - assembly]: out of range\n");
        }
    }
    public:
    bool opt=false;
    bool use_entry0_as=true;
    std::string file = "";
    void analyze();

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

    std::vector<uint8_t> compile();
};

#endif