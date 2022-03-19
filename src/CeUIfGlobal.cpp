#include "CeUIfGlobal.h"

const std::string CeUIfContentPath = IOSAbstract::GetExecutableFolderPath() + "/Content";
const std::string CeUIfDefaultHtmlFilePath = CeUIfContentPath + "/html/CeUIf.html";

const std::string& ICeUIf::GetContentPath()
{
    return CeUIfContentPath;
}

const std::string& ICeUIf::GetDefaultHtmlFilePath()
{
    return CeUIfDefaultHtmlFilePath;
}
