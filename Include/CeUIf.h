#pragma once
#include "CeUIfExport.h"
#include <string>
#include "OSAbstract.h"

enum ECeUIfMessageLoopState
{
    CMLS_NotStarted,
    CMLS_Running,
    CMLS_RequestQuit,
    CMLS_Quit,
};

struct CEUIF_API ICeUIf
{
    static int32_t RemoteDebuggingPort;

    static const std::string& GetContentPath();
    static const std::string& GetDefaultHtmlFilePath();
    

    static bool Initialize(int Argc, char* Argv[]);
    static void DoMessageLoopWork();
    static void RunMessageLoop();
    static void Uninitialize();

    static bool IsRunning();
    static bool IsQuit();
    static bool IsRequestQuit();
    static void RequestQuit();
};

