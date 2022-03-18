#include "OSAbstract.h"

#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

std::string IOSAbstract::GetExecutableFolderPath()
{
    std::string FilePath = GetExecutableFilePath();
    return FilePath.substr(0, FilePath.find_last_of("\\/") + 1);
}

std::string IOSAbstract::GetExecutableFilePath()
{
#ifdef _WIN32
    char FileName[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, FileName, MAX_PATH);
    return FileName;
#else
    char FileName[PATH_MAX];
    size_t Count = readlink("/proc/self/exe", FileName, PATH_MAX);
    return std::string(FileName, (Count > 0) ? Count : 0);
#endif
}

std::string IOSAbstract::GetExecutableFileName()
{
    std::string FilePath = GetExecutableFilePath();
    size_t Pos = FilePath.find_last_of("/\\");
    return FilePath.substr(Pos, FilePath.size() - Pos);
}
