#include "linkerlib.h"



int main(int argc, char *argv[])
{
    std::vector<std::string> files;
    std::string output = "out.bin";
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
   return link(files, output, opt, objs, global);
}