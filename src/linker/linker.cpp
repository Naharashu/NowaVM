#include "../common.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

inline uint64_t fetch64(std::vector<uint8_t> m, uint64_t i)
{
    uint64_t res = 0;
    for (int a = 0; a < 8; a++)
    {
        res |= (uint64_t)(m[i++]) << (8 * a);
    }
    return res;
}

inline std::array<uint8_t, 8> slice64(uint64_t i)
{
    std::array<uint8_t, 8> res;
    std::memcpy(res.data(), &i, sizeof(i));
    return res;
}

void writebin(const std::string &name, std::vector<uint8_t> &res)
{
    std::ofstream f(name, std::ios::out | std::ios::binary);
    f.write(reinterpret_cast<const char *>(res.data()), res.size() * sizeof(res[0]));
    f.close();
}

std::vector<uint8_t> readbin(const std::string &name)
{
    std::ifstream f(name, std::ios::in | std::ios::binary);
    if (!f.is_open())
    {
        std::cerr << "[Error]: file '" + name + "' doesnt exist!\n";
        return {};
    }
    std::vector<uint8_t> bytecode;
    uint8_t c;
    while (f.read(reinterpret_cast<char *>(&c), sizeof(c)))
    {
        bytecode.emplace_back(c);
    }
    return bytecode;
}

std::vector<std::string> split(const std::string &s)
{
    std::vector<std::string> res;
    std::istringstream iss(s);
    std::string word;
    while (iss >> word)
    {
        res.push_back(word);
    }
    return res;
}
std::vector<std::pair<uint64_t, std::string>> all_relocs_;
struct obj
{
    uint64_t begin = 0;
    uint64_t end = 0;
    std::vector<uint8_t> bytecode{};
    std::unordered_map<std::string, uint64_t> export_;
    std::vector<std::pair<uint64_t, std::string>> relocs_;
    void dead_code()
    {
        for (uint64_t i = 0; i < bytecode.size();i++)
        {
            for (auto &x : export_)
            {
                bool seen = false;
                for (auto &y : all_relocs_)
                {
                    if (y.second == x.first)
                    {
                        seen = true;
                        break;
                    }
                }
                if (!seen)
                {
                    uint64_t start = x.second;
                    uint64_t end = bytecode.size();
                    for (auto &other : export_)
                    {
                        if (other.second > start && other.second < end)
                            end = other.second;
                    }
                    for (uint64_t j = start; j < end; j++)
                        bytecode[j] = NOOP;
                }
            }
        }
    }


    void parse_sym(const std::string &name)
    {
        std::fstream f(name);
        if (!f.is_open())
        {
            std::cerr << "[Error - linker]: file '" << name << "' doesnt exist!\n";
            f.close();
            return;
        }
        std::string line;
        std::string code;
        while (std::getline(f, line))
        {
            code += line + ' ';
            if (f.eof())
                break;
        }

        std::vector<std::string> toks = split(code);
        for (uint64_t i = 0; i < toks.size();)
        {
            if (toks[i] == "EXPORT")
            {
                i++;
                export_[toks[i]] = std::stoull(toks[i + 1]);
                i += 2;
            }
            else if (toks[i] == "RELOC")
            {
                i++;
                relocs_.emplace_back(std::make_pair(std::stoull(toks[i]), toks[i + 1]));
                i += 2;
            }
        }
        /*
        for(auto &x : relocs_) {
            bool seen = false;
            for(auto &y : export_) {
                if(x.second==y.first) seen = true;
            }
            if(!seen) dead_code();
        }
        */
    }
};


int main(int argc, char *argv[])
{
    std::vector<std::string> files;
    std::string output = "a.bin";
    bool opt = false;
    std::vector<obj> objs;
    std::unordered_map<std::string, uint64_t> global;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            output = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            std::cout << "NowaVM Linker V1.1\n";
            return 0;
        }
        else if (strcmp(argv[i], "-opt") == 0)
        {
            opt = true;
        }
        else if (argv[i][0] != '-')
        {
            files.push_back(argv[i]);
        }
    }
    uint64_t offset = 0;
    for (auto &x : files)
    {
        std::vector<uint8_t> code = readbin(x);
        obj temp = {offset, offset + code.size() - 1, code};
        temp.parse_sym(x.substr(0, x.size() - 3) + "sym");
        offset += code.size();
        objs.push_back(temp);
    }
    for (auto &o : objs)
    {
        for (auto &x : o.relocs_)
        {
            all_relocs_.emplace_back(x);
        }
        for (auto &[name, local_off] : o.export_)
            global[name] = o.begin + local_off;
    }
    for (auto &o : objs)
    {
        for (auto &r : o.relocs_)
        {
            uint64_t resolved = global[r.second];
            auto bytes = slice64(resolved);
            for (int j = 0; j < 8; j++)
                o.bytecode[r.first + j] = bytes[j];
        }
    }

    if (opt)
        for (auto &o : objs)
            o.dead_code();

    std::vector<uint8_t> res;
    for (auto &x : objs)
    {
        for (auto &y : x.bytecode)
        {
            res.emplace_back(y);
        }
    }
    writebin(output, res);
    return 0;
}