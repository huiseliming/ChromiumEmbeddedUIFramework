#include <windows.h>
#include <include/cef_app.h>
#include <include/cef_command_line.h>

enum EProcessType {
    PT_BROWSER,
    PT_RENDERER,
    PT_OTHER,
};

static const char ProcessTypeCString[] = "type";
static const char RendererProcessCString[] = "renderer";
#if defined(OS_LINUX)
static const char ZygoteProcessCString[] = "zygote";
#endif

EProcessType GetProcessType(const CefRefPtr<CefCommandLine>& CommandLine)
{
    // The command-line flag won't be specified for the browser process.
    if (!CommandLine->HasSwitch(ProcessTypeCString))
        return PT_BROWSER;

    const std::string& process_type = CommandLine->GetSwitchValue(ProcessTypeCString);
    if (process_type == RendererProcessCString)
        return PT_RENDERER;

#if defined(OS_LINUX)
    // On Linux the zygote process is used to spawn other process types. Since we
    // don't know what type of process it will be we give it the renderer app.
    if (process_type == kZygoteProcess)
        return PROCESS_TYPE_RENDERER;
#endif

    return PT_OTHER;
}

class CefSubProcessApp : public CefApp
{
public:

};

class CefOtherApp : 
    public CefApp,
    public CefRenderProcessHandler
{
public:

private:
    IMPLEMENT_REFCOUNTING(CefOtherApp);
};

class CefRenderApp : public CefApp
{
public:

private:
    IMPLEMENT_REFCOUNTING(CefRenderApp);
};

int main(int Argc, char* Argv[])
{
    CefEnableHighDPISupport();
    CefRefPtr<CefCommandLine> CommandLine = CefCommandLine::CreateCommandLine();
#if defined(_WIN32)
    CefMainArgs MainArgs(GetModuleHandle(NULL));
    CommandLine->InitFromString(GetCommandLineW());
#else
    CefMainArgs MainArgs(Argc, Argv);
    CommandLine->InitFromArgv(Argc, Argv);
#endif

    CefRefPtr<CefApp> App;
    auto ProcessType = GetProcessType(CommandLine);
    if (ProcessType == EProcessType::PT_RENDERER) {
        App = new CefRenderApp();
    }
    else if (ProcessType == EProcessType::PT_OTHER) {
        App = new CefOtherApp();
    }
    else {
        return 1;
    }

    void* SandboxInfo = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo ScopedSandboxInfo;
    SandboxInfo = ScopedSandboxInfo.sandbox_info();
#endif

    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    int Result = CefExecuteProcess(MainArgs, App, SandboxInfo);
    return Result;
}