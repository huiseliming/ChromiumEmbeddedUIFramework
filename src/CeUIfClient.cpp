#include "CeUIfClient.h"

CeUIfClient* GCeUIfClient = nullptr;

void SetupDirectoryProvider(CefRefPtr<CefResourceManager> ResourceManager)
{
	if (!CefCurrentlyOn(TID_IO)) {
		// Execute on the browser IO thread.
		CefPostTask(TID_IO, base::BindOnce(SetupDirectoryProvider, ResourceManager));
		return;
	}
	std::string DirectoryPath = "file://" + IOSAbstract::GetExecutableFolderPath() + "/html";
	std::string URIPath = URI_ROOT;
	ResourceManager->AddDirectoryProvider(URIPath, DirectoryPath, 1, URIPath);
}

CeUIfClient::CeUIfClient()
	: ResourceManager(new CefResourceManager)
{
	GCeUIfClient = this;
	SetupDirectoryProvider(ResourceManager);
}
