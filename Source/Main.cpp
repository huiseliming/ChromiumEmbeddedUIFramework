
#include <windows.h>
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "CeUIfApp.h"
#include "CeUIfClient.h"


int main(int Argc, char* Argv[]) 
{
#if defined(_WIN32)
    CefEnableHighDPISupport();
#endif
    CefRefPtr<CefCommandLine> CommandLine = CefCommandLine::CreateCommandLine();

#if defined(_WIN32)
    CefMainArgs MainArgs(GetModuleHandle(NULL));
    CommandLine->InitFromString(GetCommandLineW());
#else
    CefMainArgs MainArgs(Argc, Argv);
    CommandLine->InitFromArgv(Argc, Argv);
#endif

    void* SandboxInfo = nullptr;
#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo ScopedSandboxInfo;
    SandboxInfo = ScopedSandboxInfo.sandbox_info();
#endif

    CefRefPtr<CeUIfApp> App(new CeUIfApp);

    // Populate this structure to customize CEF behavior.
    CefSettings Settings;
    Settings.remote_debugging_port = 1234;
    CefString(&Settings.browser_subprocess_path).FromString(IOSAbstract::GetExecutableFolderPath() + "/CeUIfSubProcess.exe");

#if !defined(CEF_USE_SANDBOX)
    Settings.no_sandbox = true;
#endif

    // Initialize CEF in the main process.
    CefInitialize(MainArgs, Settings, App.get(), SandboxInfo);

//    CefWindowInfo windowInfo;
//#if defined(_WIN32)
//    // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
//    windowInfo.SetAsPopup(NULL, "CeUIf");
//#endif
//    CefBrowserSettings BrowserSettings;
//    CefBrowserHost::CreateBrowser(windowInfo, new CeUIfClient, IOSAbstract::GetExecutableFolderPath() + "/html/CeUIf.html" /*CeUIfURI */ , BrowserSettings, nullptr, nullptr);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();
    // Shut down CEF.
    CefShutdown();

    return 0;
}

