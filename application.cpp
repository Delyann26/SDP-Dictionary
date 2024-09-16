#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

void displayUsage(const char* executablePath)
{
    try {
        fs::path ep(executablePath);

        std::cout
            << "Usage:\n\t"
            << ep.filename()
            << " <dictionary> <filter> <text>"
            << std::endl;
    }
    catch (...) {
        std::cout << "Cannot parse path from argv[0]";
    }
}


int main(int argc, char* argv[])
{
    if (argc < 4) {
        displayUsage(argv[0]);
        return 1;
    }
    const char* dictPath = argv[1];
    const char* filterPath = argv[2];
    const char* textPath = argv[3];

    Dictionary dict;
    dict.loadFiles(dictPath, filterPath, textPath);

    return 0;
}