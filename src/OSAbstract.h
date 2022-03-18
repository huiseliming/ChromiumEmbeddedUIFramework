#pragma once
#include <string>

struct IOSAbstract
{
    static std::string GetExecutableFolderPath();
    static std::string GetExecutableFilePath();
    static std::string GetExecutableFileName();
};
