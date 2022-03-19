#include "OSAbstract.h"
#include <algorithm>
#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

std::string IOSAbstract::GetExecutableFolderPath()
{
    std::string FilePath = GetExecutableFilePath();
    return FilePath.substr(0, FilePath.rfind('/'));
}

std::string IOSAbstract::GetExecutableFilePath()
{
#ifdef _WIN32
    char Buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, Buffer, MAX_PATH);
    std::string FilePath = Buffer;
    std::replace(FilePath.begin(), FilePath.end(), '\\', '/');
    return FilePath;
#else
    char Buffer[PATH_MAX];
    size_t Count = readlink("/proc/self/exe", Buffer, PATH_MAX);
    return std::string(Buffer, (Count > 0) ? Count : 0);
#endif
}

std::string IOSAbstract::GetExecutableFileName()
{
    std::string FilePath = GetExecutableFilePath();
    size_t Pos = FilePath.rfind('/');
    return FilePath.substr(Pos, FilePath.size() - Pos);
}
