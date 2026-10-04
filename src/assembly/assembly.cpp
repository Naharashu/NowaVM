#include "assembly.h"
#include <bit>
#include <cstdint>

std::unordered_map<std::string, uint64_t> labels;
std::unordered_set<std::string> extern_labels;
std::unordered_map<std::string, std::string> opcodes = {
    {"noop", "0x00"}, {"NOOP", "0x00"},
    {"ld", "0x01"}, {"LD", "0x01"},
    {"add", "0x02"}, {"ADD", "0x02"},
    {"sub", "0x03"}, {"SUB", "0x03"},
    {"mul", "0x04"}, {"MUL", "0x04"},
    {"div", "0x05"}, {"DIV", "0x05"},
    {"imul", "0x06"}, {"IMUL", "0x06"},
    {"idiv", "0x07"}, {"IDIV", "0x07"},
    {"xor", "0x08"}, {"XOR", "0x08"},
    {"and", "0x09"}, {"AND", "0x09"},
    {"or", "0x0A"}, {"OR", "0x0A"},
    {"shl", "0x0B"}, {"SHL", "0x0B"},
    {"shr", "0x0C"}, {"SHR", "0x0C"},
    {"jmp", "0x0D"}, {"JMP", "0x0D"},
    {"cmp", "0x0E"}, {"CMP", "0x0E"},
    {"jz", "0x0F"}, {"JZ", "0x0F"},
    {"jnz", "0x10"}, {"JNZ", "0x10"},
    {"jc", "0x11"}, {"JC", "0x11"},
    {"jnc", "0x12"}, {"JNC", "0x12"},
    {"store", "0x13"}, {"STORE", "0x13"},
    {"ldm", "0x14"}, {"LDM", "0x14"},
    {"jl", "0x15"}, {"JL", "0x15"},
    {"jle", "0x16"}, {"JLE", "0x16"},
    {"jb", "0x17"}, {"JB", "0x17"},
    {"jbe", "0x18"}, {"JBE", "0x18"},
    {"jmprv", "0x19"}, {"JMPRV", "0x19"},
    {"push", "0x1A"}, {"PUSH", "0x1A"},
    {"pop", "0x1B"}, {"POP", "0x1B"},
    {"call", "0x1C"}, {"CALL", "0x1C"},
    {"ret", "0x1D"}, {"RET", "0x1D"},
    {"fadd", "0x1E"}, {"FADD", "0x1E"},
    {"fsub", "0x1F"}, {"FSUB", "0x1F"},
    {"fmul", "0x20"}, {"FMUL", "0x20"},
    {"fdiv", "0x21"}, {"FDIV", "0x21"},
    {"copy", "0x22"}, {"COPY", "0x22"},
    {"swap", "0x23"}, {"SWAP", "0x23"},
    {"fma", "0x24"}, {"FMA", "0x24"},
    {"ltf", "0x25"}, {"LTF", "0x25"},
    {"ftl", "0x26"}, {"FTL", "0x26"},
    {"not", "0x27"}, {"NOT", "0x27"},
    {"ror", "0x28"}, {"ROR", "0x28"},
    {"rol", "0x29"}, {"ROL", "0x29"},
    {"arx", "0x2A"}, {"ARX", "0x2A"},
    {"storx", "0x2B"}, {"STORX", "0x2B"},
    {"ldmx", "0x2C"}, {"LDMX", "0x2C"},
    {"ldzero", "0x2D"}, {"LDZERO", "0x2D"},
    {"print_reg", "0x2E"}, {"PRINT_REG", "0x2E"},
    {"input_reg", "0x2F"}, {"INPUT_REG", "0x2F"},
    {"neg", "0x30"}, {"NEG", "0x30"},
    {"inc", "0x31"}, {"INC", "0x31"},
    {"dec", "0x32"}, {"DEC", "0x32"},
    {"vaddqw", "0x33"}, {"VADDQW", "0x33"},
    {"vsubqw", "0x34"}, {"VSUBQW", "0x34"},
    {"vdivqw", "0x35"}, {"VDIVQW", "0x35"},
    {"vmulqw", "0x36"}, {"VMULQW", "0x36"},
    {"vcmp", "0x37"}, {"VCMP", "0x37"},
    {"vshlqw", "0x38"}, {"VSHLQW", "0x38"},
    {"vshrqw", "0x39"}, {"VSHRQW", "0x39"},
    {"vxor", "0x3A"}, {"VXOR", "0x3A"},
    {"vand", "0x3B"}, {"VAND", "0x3B"},
    {"vor", "0x3C"}, {"VOR", "0x3C"},
    {"vcopy", "0x3D"}, {"VCOPY", "0x3D"},
    {"vswap", "0x3E"}, {"VSWAP", "0x3E"},
    {"vldqw", "0x3F"}, {"VLDQW", "0x3F"},
    {"vlddw", "0x40"}, {"VLDDW", "0x40"},
    {"vldw", "0x41"}, {"VLDW", "0x41"},
    {"vld", "0x42"}, {"VLD", "0x42"},
    {"vldsp", "0x43"}, {"VLDSP", "0x43"},
    {"vlddp", "0x44"}, {"VLDDP", "0x44"},
    {"vldmxqw", "0x45"}, {"VLDMXQW", "0x45"},
    {"vldlqw", "0x46"}, {"VLDLQW", "0x46"},
    {"vnot", "0x47"}, {"VNOT", "0x47"},
    {"vadddw", "0x48"}, {"VADDQW", "0x48"},
    {"vsubdw", "0x49"}, {"VSUBQW", "0x49"},
    {"vdivdw", "0x4A"}, {"VDIVQW", "0x4A"},
    {"vmuldw", "0x4B"}, {"VMULQW", "0x4B"},
    {"vaddw", "0x4C"}, {"VADDQW", "0x4C"},
    {"vsubw", "0x4D"}, {"VSUBQW", "0x4D"},
    {"vdivw", "0x4E"}, {"VDIVQW", "0x4E"},
    {"vmulw", "0x4F"}, {"VMULQW", "0x4F"},
    {"vadd", "0x50"}, {"VADDQW", "0x50"},
    {"vsub", "0x51"}, {"VSUBQW", "0x51"},
    {"vdiv", "0x52"}, {"VDIVQW", "0x52"},
    {"vmul", "0x53"}, {"VMULQW", "0x53"},
    {"vaddsp", "0x54"}, {"VADDSP", "0x54"},
    {"vsubsp", "0x55"}, {"VSUBSP", "0x55"},
    {"vdivsp", "0x56"}, {"VDIVSP", "0x56"},
    {"vmulsp", "0x57"}, {"VMULSP", "0x57"},
    {"vadddp", "0x58"}, {"VADDDP", "0x58"},
    {"vsubdp", "0x59"}, {"VSUBDP", "0x59"},
    {"vdivdp", "0x5A"}, {"VDIVDP", "0x5A"},
    {"vmuldp", "0x5B"}, {"VMULDP", "0x5B"},
    {"vstregqw", "0x5C"}, {"VMSTREGQW", "0x5C"},
    {"ext", "0xFE"}, {"EXTENSION", "0xFE"},
    {"hlt", "0xFF"}, {"HLT", "0xFF"},
};

static bool is_vreg_name(const std::string &id)
{
    if (id.size() < 3)
        return false;
    if ((id[0] != 'v' && id[0] != 'V') || (id[1] != 'r' && id[1] != 'R'))
        return false;
    return std::all_of(id.begin() + 2, id.end(), ::isdigit);
}

static bool is_vext_opcode_value(const std::string &value)
{
    static const std::unordered_set<std::string> vext_values = {
        "0x33", "0x34", "0x35", "0x36", "0x37", "0x38", "0x39",
        "0x3A", "0x3B", "0x3C", "0x3D", "0x3E", "0x3F", "0x40",
        "0x41", "0x42", "0x43", "0x44", "0x45"
    };
    return vext_values.contains(value);
}

bool lexer::is_opcode(const std::string &id)
{
    return opcodes.contains(id);
}


std::string lexer::preprocessor(const std::string &fname)
{
    std::string res;
    std::string line;
    if (use_entry0)
    {
        std::ifstream entry0(std::string{std::getenv("HOME")} + "/.local/bin/include_nowavm/entry0.asm");
        while (std::getline(entry0, line))
        {
            res += line + '\n';
        }
    }
    line = "";
    std::string finame = std::filesystem::path(fname).lexically_normal().string();
    if (included.contains(finame))
        return "";
    included.insert(finame);
    std::ifstream f(finame);
    if (!f.is_open())
        throw assembly_error("[Error - assembly]: file '" + finame + "' doesnt exitst\n");
    auto trim = [](std::string &s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");

        if (start == std::string::npos)
        {
            s.clear();
            return;
        }

        s = s.substr(start, end - start + 1);
    };

    auto split = [](const std::string &s)
    {
        std::vector<std::string> res;
        std::istringstream iss(s);
        std::string word;
        while (iss >> word)
        {
            res.push_back(word);
        }
        return res;
    };
    while (std::getline(f, line))
    {
        if (auto pos = line.find(';'); pos != std::string::npos)
            line = line.substr(0, pos);
        if (line.rfind("#ifdef", 0) == 0)
        {
            std::string name = line.substr(6);
            trim(name);
            active_stack.push_back(active);
            active = active && defined.contains(name);
        }
        else if (line.rfind("#ifndef", 0) == 0)
        {
            std::string name = line.substr(7);
            trim(name);
            active_stack.push_back(active);
            active = active && !defined.contains(name);
        }
        else if (line.rfind("#ifeq", 0) == 0)
        {
            std::string str = line.substr(5);
            auto parts = split(str);
            bool eq = false;
            if (parts.size() >= 2) {
                std::string val1 = defined.contains(parts[0]) ? defined[parts[0]] : parts[0];
                std::string val2 = defined.contains(parts[1]) ? defined[parts[1]] : parts[1];
                eq = (val1 == val2);
            }
            active_stack.push_back(active);
            active = active && eq;
        }
        else if (line.rfind("#ifneq", 0) == 0)
        {
            std::string str = line.substr(6);
            auto parts = split(str);
            bool eq = false;
            if (parts.size() >= 2) {
                std::string val1 = defined.contains(parts[0]) ? defined[parts[0]] : parts[0];
                std::string val2 = defined.contains(parts[1]) ? defined[parts[1]] : parts[1];
                eq = (val1 != val2);
            }
            active_stack.push_back(active);
            active = active && eq;
        }
        else if (line.rfind("#else", 0) == 0)
        {
            if (active_stack.empty())
                throw assembly_error("[Error - preprocessor]: unexpected #else\n");
            active = active_stack.back() && !active;
        }
        else if (line.rfind("#endif", 0) == 0)
        {
            if (active_stack.empty())
                throw assembly_error("[Error - preprocessor]: unexpected #endif\n");
            active = active_stack.back();
            active_stack.pop_back();
        }
        else if (active)
        {
            if (line.rfind("#include", 0) == 0)
            {
                std::string include = line.substr(8);
                trim(include);
                include.erase(0, include.find_first_not_of(" \t"));

                res += preprocessor(include);
            }
            else if (line.rfind("#define", 0) == 0)
            {
                std::string def = line.substr(7);
                std::string name;
                std::string val;
                bool name_seen = false;
                bool val_seen = false;
                for (uint32_t i = 0; i < def.size();)
                {
                    uint8_t c = def[i];
                    if ((is_letter(c) || c == '_'))
                    {
                        while (i < def.size() && (is_letter(def[i]) || is_int(def[i]) || def[i] == '_'))
                        {
                            name.push_back(def[i]);
                            i++;
                        }
                        name_seen = true;
                    }
                    else if (is_int(c))
                    {
                        if (!name_seen)
                        {
                            throw assembly_error("[Error - preprocessor]: expected NAME for macro(e.g #define ERROR "
                                                 "1), but got immediate\n");
                        }
                        while (i < def.size())
                        {
                            val.push_back(def[i]);
                            i++;
                        }
                        val_seen = true;
                        defined[name] = val;
                        break;
                    }
                    else
                        i++;
                }
                if ((!val_seen) && name_seen)
                    defined[name] = "1";
                else if (!val_seen && !name_seen)
                    throw assembly_error("[Error - preprocessor]: expected NAME for macro(e.g. #define DEBUG)\n");
            }
            else if (line.rfind("#undef", 0) == 0)
            {
                std::string macro_name = line.substr(6);
                trim(macro_name);
                if (defined.contains(macro_name))
                    defined.erase(defined.find(macro_name));
            }
            else if (line.rfind("#warn", 0) == 0)
            {
                std::string msg = line.substr(5);
                trim(msg);
                msg.erase(0, msg.find_first_not_of(" \t"));
                std::cerr << "[Warning]: " << msg << '\n';
            }
            else if (line.rfind("#error", 0) == 0)
            {
                std::string msg = line.substr(6);
                trim(msg);
                msg.erase(0, msg.find_first_not_of(" \t"));
                throw assembly_error("[Error - preprocessor]: " + msg + '\n');
            }
            else if (line.rfind("#extern", 0) == 0)
            {
                std::string name = line.substr(7);
                trim(name);
                name.erase(0, name.find_first_not_of(" \t"));
                labels[name] = 0;
                extern_labels.emplace(name);
            }
            else
                res += line + '\n';
        }
    }
    return res;
}
void lexer::collect_labels()
{
    uint64_t address = 0;
    for (uint64_t i = 0; i < code.size();)
    {
        uint8_t c = code[i];
        if (is_letter(c) || c == '_')
        {
            std::string id;
            while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_' || code[i] == ':'))
            {
                id.push_back(code[i]);
                i++;
            }
            if (!id.empty() && id.back() == ':')
            {
                id.pop_back();
                labels.insert_or_assign(id, address);
            }
            else if (is_opcode(id))
            {
                address++;
            }
            else if (is_vreg_name(id))
            {
                address++;
            }
            else if ((id[0] == 'R' || id[0] == 'r') && std::all_of(id.begin() + 1, id.end(), ::isdigit))
            {
                address++;
            }
            else
            {
                address += 8;
            }
        }
        else if (is_int(c))
        {
            while (i < code.size() && (is_int(code[i])||code[i]=='.'||code[i]=='f'))
            {
                i++;
            }
            address += 8;
        }
        else if (c == ';')
        {
            i++;
            while (i < code.size() && code[i] != '\n')
                i++;
        }
        else
            i++;
    }
}
void lexer::lex()
{

    uint64_t l = 0;
    uint16_t c = 0;
    uint64_t addr = 0;
    collect_labels();
    for (uint64_t i = 0; i < code.size();)
    {
        uint8_t s = code[i];
        if (s == ';')
        {
            i++;
            while (i < code.size() && code[i] != '\n')
                i++;
        }
        else if (i < code.size() && s == '\n')
        {
            l++;
            c = 0;
            i++;
        }
        else if (s == '\t' || s == '\r' || s == ' ')
        {
            c++;
            i++;
        }
        else if (is_letter(s) || s == '_')
        {
            std::string id;
            while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_' || code[i] == ':'))
            {
                id.push_back(code[i]);
                i++;
                c++;
            }
            if (!id.empty() && id.back() == ':')
            {
                id.pop_back();
                continue;
            }
            if (is_vreg_name(id))
            {
                std::string_view n(id.data() + 2, id.size() - 2);
                lexed.emplace_back(token{REGN, l, c, std::string{n}, addr});
                addr++;
                continue;
            }
            if ((id[0] == 'R' || id[0] == 'r') && std::all_of(id.begin() + 1, id.end(), ::isdigit))
            {
                std::string_view n(id.data() + 1, id.size() - 1);
                lexed.emplace_back(token{REGN, l, c, std::string{n}, addr});
                addr++;
                continue;
            }
            if (id == "include" || id == "define")
            {
                while (i < code.size() && code[i] != '\n')
                {
                    i++;
                    c++;
                }
                continue;
            }
            std::string opcode = opcodes[id];
            if (defined.contains(id))
            {
                std::string val = defined.at(id);
                if(std::all_of(val.begin() + 1, val.end(), ::isdigit)) {
                    lexed.emplace_back(token{INT, l, c, defined.at(val), addr});
                    addr += 8;
                } else if((val[0] == 'R' || val[0] == 'r') && std::all_of(val.begin() + 1, val.end(), ::isdigit)) {
                    lexed.emplace_back(token{REGN, l, c, val.substr(1), addr});
                    addr++;
                } else if(is_opcode(id)) {
                    lexed.emplace_back(token{ID, l, c, opcodes[val], addr});
                    addr++;
                } else {
                    lexed.emplace_back(token{ID, l, c, val, addr});
                    addr++;
                }
                continue;
            }
            if (opcode == "" && !labels.contains(id))
            {
                if(id=="_start") {
                    throw assembly_error("[Error - assembly:" + filename + ':' + std::to_string(l) + ':' +
                                     std::to_string(c) + "]: not found entry point _start in code, use -fno-entry0 to silent this error\n");
                }
                else throw assembly_error("[Error - assembly:" + filename + ':' + std::to_string(l) + ':' +
                                     std::to_string(c) + "]: unknown symbol '" + id + "' found in code\n");
            }
            if (opcode == "")
            {
                lexed.emplace_back(token{LABEL, l, c, id, addr});
                addr += 8;
                continue;
            }
            lexed.emplace_back(token{ID, l, c, opcode, addr});
            addr++;
        }
        else if (is_int(s))
        {
            std::string number;
            bool seen_dot=false;
            bool seen_f=false;
            while (i < code.size() && (is_int(code[i])||code[i]=='.'||code[i]=='f'))
            {
                if(seen_f) break;
                if(code[i]=='.') {
                    if(!seen_dot) seen_dot = true;
                    else break;
                }
                if(code[i]=='f') seen_f = true;
                number.push_back(code[i]);
                i++;
                c++;
            }
            if(seen_dot) {
                if(seen_f) lexed.emplace_back(token{FLOAT, l, c, number, addr});
                else lexed.emplace_back(token{DOUBLE, l, c, number, addr});
            }
            else lexed.emplace_back(token{INT, l, c, number, addr});
            addr += 8;
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
void assembly::analyze()
{
    if (!opt)
        return;
    std::array<uint8_t, 256> register_usage{};
    for (auto &x : lexed)
    {
        if (x.t == REGN)
            register_usage[std::stoull(x.val) % 256]++;
    }
    uint64_t s = lexed.size() - 2;
    for (uint64_t i = 0; i < s; i++)
    {
        token c = lexed[i];
        token n = lexed[i + 1];
        token ah = lexed[i + 2];
        if (c.t == ID && n.t == ID && n.line == c.line)
        {
            throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) +
                                 "]: expected register or immediate after '" + c.val + "', but got '" + n.val + "'\n");
        }
        else if (c.t == INT && n.t == INT)
        {
            throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) +
                                 "]: expected end of instruction, but got '" + n.val + "'\n");
        } //else if(c.t==ID&&n.t==REGN&&ah.t==ID)
        else if (c.val == "0xFF" && n.t != EOF_)
        {
            lexed[i + 1] = token{EOF_, 0, 0, "end", i+1};
            break;
        }
        else if (c.val == "0x0D" && n.t == LABEL)
        {
            // jmp label 0 1..8
            // label:
            // ->
            // noop (just executes linearly)
            auto x = labels.find(n.val);
            if (x != labels.end() && ah.address == x->second)
            {
                lexed[i] = {EMPTY, 0, 0, "null", 0};
                lexed[i + 1] = {EMPTY, 0, 0, "null", 0};
            }
        }
        else if (c.val == "0x01" && (n.t == REGN && register_usage[std::stoull(n.val)] == 1))
        {
            // ld r0 42
            // ->
            // noop (never used)
            lexed[i] = {EMPTY, 0, 0, "null", 0};
            lexed[i + 1] = {EMPTY, 0, 0, "null", 0};
            lexed[i + 2] = {EMPTY, 0, 0, "null", 0};
        }
        else if ((c.val == "0x08" || c.val == "0x03") && (n.t == REGN && ah.t == REGN) && n.val == ah.val)
        {
            lexed[i].val = "0x2D";
            std::cout << lexed[i].val << ' ';
            lexed[i + 2] = {EMPTY, 0, 0, "null", 0};
        }
        i += 2;
    }
}
std::vector<uint8_t> assembly::compile()
{
    std::string linker_sym_name = file.substr(0, file.size() - 3) + "sym";
    std::ofstream f(linker_sym_name);
    std::string cmd = "rm -f " + linker_sym_name;
    if(no_sym) system(cmd.c_str());
    std::vector<uint8_t> compiled;
    analyze();
    while (indx < lexed.size() && peek().t != EOF_)
    {
        if (lexed[indx].t == ID)
        {
            std::string id = lexed[indx].val;
            if (id == "0xFE")
            {
                compiled.emplace_back(0xFE);
                compiled.emplace_back(0x01);
                consume();
                continue;
            }
            if (is_vext_opcode_value(id))
            {
                compiled.emplace_back(0xFE);
                compiled.emplace_back(0x01);
                compiled.emplace_back(std::stoul(id, 0, 16));
                consume();
                continue;
            }
            compiled.emplace_back(std::stoul(id, 0, 16));
            consume();
            continue;
        }
        else if (peek().t == INT)
        {
            uint64_t val = std::stoull(lexed[indx].val);
            consume();
            std::array<uint8_t, 8> bytes = slice64(val);
            for (auto &x : bytes)
            {
                compiled.emplace_back(x);
            }
        } else if(peek().t==FLOAT||peek().t==DOUBLE) {
            uint64_t val = std::bit_cast<uint64_t>(std::stod(lexed[indx].val));
            consume();
            std::array<uint8_t, 8> bytes = slice64(val);
            for (auto &x : bytes)
            {
                compiled.emplace_back(x);
            }
        }
        else if (peek().t == REGN)
        {
            compiled.emplace_back(std::stol(lexed[indx].val) % 256);
            consume();
        }
        else if (peek().t == LABEL)
        {
            std::string id = peek().val;
            if(!no_sym) f << "RELOC " << compiled.size() << ' ' << id << '\n';
            uint64_t val = labels.at(id);
            consume();
            std::array<uint8_t, 8> bytes = slice64(val);
            for (auto &x : bytes)
            {
                compiled.emplace_back(x);
            }
        }
        else if (peek().t == MACRO)
        {
            uint64_t val = stoull(peek().val);
            consume();
            std::array<uint8_t, 8> bytes = slice64(val);
            for (auto &x : bytes)
            {
                compiled.emplace_back(x);
            }
        }
        else
        {
            consume();
        }
    }
    for (auto &x : labels)
    {
        if (!extern_labels.contains(x.first)&&!no_sym)
            f << "EXPORT " << x.first << ' ' << x.second << '\n';
    }
    f.close();
    return compiled;
}
