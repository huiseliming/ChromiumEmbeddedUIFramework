#pragma once

#include <include/cef_app.h>

class CeUIfApp : public CefApp, public CefRenderProcessHandler
{
public:
	CeUIfApp() = default;
    
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
    {
        return this;
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





