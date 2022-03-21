#pragma once
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_resource_manager.h>
#include <include/cef_parser.h>
#include <queue>
#include "CeUIf.h"

// Returns a data: URI with the specified contents.
inline std::string GetDataURI(const std::string& data, const std::string& mime_type) {
	return "data:" + mime_type + ";base64," +
		CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
		.ToString();
}

struct CefSavedWindowInfo
{
	bool Maximized;
	LONG Style;
	LONG ExStyle;
	RECT WindowRect;
};

struct CefBrowserData
{
	bool Fullscreen;
	CefSavedWindowInfo SavedWindowInfo;
	std::shared_ptr<CefWindowInfo> WindowInfo;
};

class CeUIfClient :
	public CefClient, 
	public CefLifeSpanHandler, 
	public CefRequestHandler, 
	public CefResourceRequestHandler,
	public CefLoadHandler,
	public CefDisplayHandler,
	public CefKeyboardHandler
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
		CEF_REQUIRE_UI_THREAD();
		BrowserIdentifierToBrowserData.insert(std::make_pair(browser->GetIdentifier(), CefBrowserData{}));
		BrowserIdentifierToBrowserData[browser->GetIdentifier()].WindowInfo = WindowInfoQueue.front();
		WindowInfoQueue.pop();
		BrowserIdentifierToBrowserData[browser->GetIdentifier()].Fullscreen = false;
		if (BrowserList.empty())
		{
			OnFullscreenModeChange(browser, true);
		}
		BrowserList.push_back(browser);
	}

	void OnBeforeClose(CefRefPtr<CefBrowser> browser) override
	{
		CEF_REQUIRE_UI_THREAD();
		for (auto It = BrowserList.begin(); It != BrowserList.end(); It++)
		{
			if ((*It)->IsSame(browser))
			{
				BrowserList.erase(It);
				break;
			}
		}
		if (BrowserList.empty())
			ICeUIf::RequestQuit();
	}


	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) override
	{
		CEF_REQUIRE_UI_THREAD();

		// Don't display an error for downloaded files.
		if (errorCode == ERR_ABORTED)
			return;

		// Display a load error message using a data: URI.
		std::stringstream ss;
		ss << "<html><body bgcolor=\"white\">"
			"<h2>Failed to load URL "
			<< std::string(failedUrl) << " with error " << std::string(errorText)
			<< " (" << errorCode << ").</h2></body></html>";

		frame->LoadURL(GetDataURI(ss.str(), "text/html"));
	}

	// CefDisplayHandler methods:
	virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override
	{
		static RECT preWinRect = { 0 };
		HWND Wnd =browser->GetHost()->GetWindowHandle();
		if (Wnd == nullptr) return;
		CefSavedWindowInfo& SavedWindowInfo = BrowserIdentifierToBrowserData[browser->GetIdentifier()].SavedWindowInfo;
		if (!BrowserIdentifierToBrowserData[browser->GetIdentifier()].Fullscreen)
		{
			SavedWindowInfo.Maximized = !!::IsZoomed(Wnd);
			if (SavedWindowInfo.Maximized)
			      ::SendMessage(Wnd, WM_SYSCOMMAND, SC_RESTORE, 0);
			SavedWindowInfo.Style = GetWindowLong(Wnd, GWL_STYLE);
			SavedWindowInfo.ExStyle = GetWindowLong(Wnd, GWL_EXSTYLE);
			GetWindowRect(Wnd, &SavedWindowInfo.WindowRect);
		}
		BrowserIdentifierToBrowserData[browser->GetIdentifier()].Fullscreen = fullscreen;
		if (fullscreen)
		{
			LONG WindowStyle = GetWindowLong(Wnd, GWL_STYLE);
			SetWindowLong(Wnd, GWL_STYLE, WindowStyle & ~(WS_CAPTION | WS_THICKFRAME));
			LONG WindowExStyle = GetWindowLong(Wnd, GWL_STYLE);
			SetWindowLong(Wnd, GWL_EXSTYLE, WindowExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
			MONITORINFO MonitorInfo;
			MonitorInfo.cbSize = sizeof(MonitorInfo);
			GetMonitorInfo(MonitorFromWindow(Wnd, MONITOR_DEFAULTTONEAREST), &MonitorInfo);
			SetWindowPos(Wnd, NULL,
				MonitorInfo.rcMonitor.left, 
				MonitorInfo.rcMonitor.top,
				MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
				MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}
		else
		{
			SetWindowLong(Wnd, GWL_STYLE, SavedWindowInfo.Style);
			SetWindowLong(Wnd, GWL_EXSTYLE, SavedWindowInfo.ExStyle);

			SetWindowPos(Wnd, NULL, 
				SavedWindowInfo.WindowRect.left,
				SavedWindowInfo.WindowRect.top,
				SavedWindowInfo.WindowRect.right - SavedWindowInfo.WindowRect.left,
				SavedWindowInfo.WindowRect.bottom - SavedWindowInfo.WindowRect.top,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			if (SavedWindowInfo.Maximized)
				::SendMessage(Wnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
	}

	std::list<CefRefPtr<CefBrowser>>& GetBrowserList()
	{
		return BrowserList;
	}

	void PushWindowInfo(std::shared_ptr<CefWindowInfo> WindowInfoPtr)
	{
		WindowInfoQueue.push(WindowInfoPtr);
	}

private:
	std::list<CefRefPtr<CefBrowser>> BrowserList;
	std::map<int, CefBrowserData> BrowserIdentifierToBrowserData;

	std::queue<std::shared_ptr<CefWindowInfo>> WindowInfoQueue;


	CefRefPtr<CefResourceManager> ResourceManager;

	IMPLEMENT_REFCOUNTING(CeUIfClient);
	DISALLOW_COPY_AND_ASSIGN(CeUIfClient);
};

extern CeUIfClient* GCeUIfClient;






