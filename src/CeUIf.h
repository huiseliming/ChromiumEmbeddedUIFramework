#pragma once
#include "CeUIfExport.h"
#include <string>
#include <Include/cef_command_line.h>
#include "OSAbstract.h"

struct CEUIF_API ICeUIf
{
    static int32_t RemoteDebuggingPort;

    static const std::string& GetContentPath();
    static const std::string& GetDefaultHtmlFilePath();

    static bool Initialize(int Argc, char* Argv[]);
    static void DoMessageLoopWork();
    static void RunMessageLoop();
    static void Uninitialize();

};

