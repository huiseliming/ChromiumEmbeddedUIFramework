#include "CeUIfClient.h"

CeUIfClient* GCeUIfClient = nullptr;

CeUIfClient::CeUIfClient()
	: ResourceManager(new CefResourceManager)
{
	GCeUIfClient = this;
	std::string DirectoryPath = IOSAbstract::GetExecutableFolderPath() + "/html";
	std::string URIPath = URI_ROOT;
	ResourceManager->AddDirectoryProvider(URIPath, DirectoryPath, 1, DirectoryPath);
}
