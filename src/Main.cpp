
#include <windows.h>
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "include/cef_app.h"


int main(int Argc, char* Argv[]) 
{
	CefRefPtr<CefCommandLine> CommandLine = CefCommandLine::CreateCommandLine();

#if defined(_WIN32)
	CefEnableHighDPISupport();
	CefMainArgs args(GetModuleHandle(NULL));
	CommandLine->InitFromString(GetCommandLineW());
#else
	CefMainArgs args(argc, argv);
	CommandLine->InitFromArgv(argc, argv);
#endif
    void* SandboxInfo = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo ScopedSandboxInfo;
    SandboxInfo = ScopedSandboxInfo.sandbox_info();
#endif

    CefRefPtr<CefApp> app = nullptr;
    std::string appType = CommandLine->GetSwitchValue("type");
    if (appType == "renderer" || appType == "zygote")
    {
        app = new RendererApp;
        // use nullptr for other process types
    }
    int result = CefExecuteProcess(args, app, SandboxInfo);
    if (result >= 0)
    {
        // child process completed
        return result;
    }

    CefSettings settings;
    settings.remote_debugging_port = 1234;
#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif

    CefInitialize(args, settings, nullptr, SandboxInfo);

    CefWindowInfo windowInfo;

#if defined(_WIN32)
    // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
    windowInfo.SetAsPopup(NULL, "simple");
#endif
    CefBrowserSettings BrowserSettings;
    CefBrowserHost::CreateBrowser(windowInfo, new MinimalClient, "CEUIF.html", BrowserSettings, nullptr, nullptr);

    CefRunMessageLoop();

    CefShutdown();

    return 0;
}

