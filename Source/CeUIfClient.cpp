#include "CeUIfClient.h"

CeUIfClient* GCeUIfClient = nullptr;

void SetupDirectoryProvider(CefRefPtr<CeUIfClient> Client, CefRefPtr<CefResourceManager> ResourceManager)
{
	if (!CefCurrentlyOn(TID_IO)) {
		// Execute on the browser IO thread.
		CefPostTask(TID_IO, base::BindOnce(SetupDirectoryProvider, Client,  ResourceManager));
		return;
	}
	const std::string DirectoryPath = ICeUIf::GetContentPath();
	const std::string URIName = "CeUIf";
	const std::string URIPath = "http://" + URIName;
	ResourceManager->AddDirectoryProvider(URIPath, DirectoryPath, 1, URIName);
}

CeUIfClient::CeUIfClient()
	: ResourceManager(new CefResourceManager)
{
	GCeUIfClient = this;
	//SetupDirectoryProvider(this, ResourceManager);
}
