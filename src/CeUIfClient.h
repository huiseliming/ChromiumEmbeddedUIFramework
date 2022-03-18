#pragma once
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_resource_manager.h>
#include "OSAbstract.h"

#define URI_ROOT "http://CeUIf"

class CeUIfClient : 
	public CefClient, 
	public CefLifeSpanHandler, 
	public CefRequestHandler, 
	public CefResourceRequestHandler
{
public:
	CeUIfClient();

	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
	CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

	CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
		CefRefPtr<CefBrowser> /*browser*/,
		CefRefPtr<CefFrame> /*frame*/,
		CefRefPtr<CefRequest> /*request*/,
		bool /*is_navigation*/,
		bool /*is_download*/,
		const CefString& /*request_initiator*/,
		bool& /*disable_default_handling*/) override {
		return this;
	}

	cef_return_value_t OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback) override
	{
		return ResourceManager->OnBeforeResourceLoad(browser, frame, request, callback);
	}

	CefRefPtr<CefResourceHandler> GetResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) override
	{
		return ResourceManager->GetResourceHandler(browser, frame, request);
	}

	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> /*browser*/,
		CefRefPtr<CefFrame> frame,
		CefProcessId /*source_process*/,
		CefRefPtr<CefProcessMessage> message) override
	{
		return false;
	}

	void OnAfterCreated(CefRefPtr<CefBrowser> browser) override
	{
		BrowserList.push_back(browser);
	}

	void OnBeforeClose(CefRefPtr<CefBrowser> browser) override
	{
		for (auto It = BrowserList.begin(); It != BrowserList.end(); It++)
		{
			if ((*It)->IsSame(browser))
			{
				BrowserList.erase(It);
				break;
			}
		}
		if (BrowserList.empty()) 
			CefQuitMessageLoop();
	}

private:
	std::list<CefRefPtr<CefBrowser>> BrowserList;

	CefRefPtr<CefResourceManager> ResourceManager;

	IMPLEMENT_REFCOUNTING(CeUIfClient);
	DISALLOW_COPY_AND_ASSIGN(CeUIfClient);
};

extern CeUIfClient* GCeUIfClient;






