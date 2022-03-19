#pragma once
#include <string>
#include "OSAbstract.h"

struct ICeUIf
{
    static const std::string& GetContentPath();
    static const std::string& GetDefaultHtmlFilePath();
};
