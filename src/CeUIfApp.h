#pragma once

#include <include/cef_app.h>
#include <include/wrapper/cef_helpers.h>
#include "CeUIfClient.h"

class CeUIfApp : 
    public CefApp, 
    public CefRenderProcessHandler, 
    public CefBrowserProcessHandler
{
public:
	CeUIfApp() = default;

    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override 
    {
        return this;
    }
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
    {
        return this;
    }

    virtual void OnContextInitialized() override
    {
        CEF_REQUIRE_UI_THREAD();
        CefWindowInfo WindowInfo;
#if defined(_WIN32)
        // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
        WindowInfo.SetAsPopup(NULL, "CeUIf");
#endif
        CefBrowserSettings BrowserSettings;
        // Create the first browser window.

        std::string DefaultURL = ICeUIf::GetContentPath() + "/html/CeUIf.html";
        CefBrowserHost::CreateBrowser(WindowInfo, new CeUIfClient, DefaultURL, BrowserSettings, nullptr, nullptr);
    }

    virtual void OnContextCreated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) 
    {
    }

    virtual void OnContextReleased(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) 
    {
    }

private:

    IMPLEMENT_REFCOUNTING(CeUIfApp);
    DISALLOW_COPY_AND_ASSIGN(CeUIfApp);
};

extern CeUIfApp* GCeUIfApp;





