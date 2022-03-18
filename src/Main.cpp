
#include <windows.h>
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "CeUIfApp.h"
#include "CeUIfClient.h"

const char* const CeUIfURI = URI_ROOT "/CeUIf.html";
const char* const CeUIfPath = "/html/CeUIf.html";

int main(int Argc, char* Argv[]) 
{
	CefRefPtr<CefCommandLine> CommandLine = CefCommandLine::CreateCommandLine();

#if defined(_WIN32)
	CefEnableHighDPISupport();
    // Structure for passing command-line arguments.
    // The definition of this structure is platform-specific.
	CefMainArgs MainArgs(GetModuleHandle(NULL));
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

    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    int Result = CefExecuteProcess(MainArgs, nullptr, SandboxInfo);
    if (Result >= 0)
    {
        // child process completed
        return Result;
    }

    // Populate this structure to customize CEF behavior.
    CefSettings Settings;
    Settings.remote_debugging_port = 1234;
#if !defined(CEF_USE_SANDBOX)
    Settings.no_sandbox = true;
#endif
    // Optional implementation of the CefApp interface.
    CefRefPtr<CeUIfApp> App(new CeUIfApp);

    // Initialize CEF in the main process.
    CefInitialize(MainArgs, Settings, App.get(), SandboxInfo);

    CefWindowInfo windowInfo;
#if defined(_WIN32)
    // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
    windowInfo.SetAsPopup(NULL, "CeUIf");
#endif
    CefBrowserSettings BrowserSettings;
    CefBrowserHost::CreateBrowser(windowInfo, new CeUIfClient, IOSAbstract::GetExecutableFolderPath() + "/html/CeUIf.html" /*CeUIfURI */ , BrowserSettings, nullptr, nullptr);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();
    // Shut down CEF.
    CefShutdown();

    return 0;
}

