#ifndef PLUGINCOMMON_H
#define PLUGINCOMMON_H

#if defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define DLLEXPORT __attribute__((visibility("default")))
#elif defined(__linux__)
#define DLLEXPORT __attribute__((visibility("default")))
#endif

#include <string>
#include <nlohmann/json.hpp>

namespace
{
	std::vector<char *> allocated;
}

void writeJSONToChar(
	char *&dest,
	const nlohmann::json &content)
{
	const std::string contentStr = content.dump(-1, ' ', true);
	dest = reinterpret_cast<char *>(malloc(sizeof(std::string::value_type) * (contentStr.size() + 1)));
	strncpy(dest, contentStr.data(), contentStr.size() + 1);
	allocated.push_back(dest);
}

extern "C" DLLEXPORT void deallocate()
{
	for (auto &ptr : allocated)
	{
		free(ptr);
	}
	allocated.clear();
}

extern "C" DLLEXPORT void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar);

extern "C" DLLEXPORT void pluginProcess(
	const char *&configCoreChar,
	const char *&configRoomChar,
	const char *&parameterChar,
	char *&configCorePatchChar,
	char *&configRoomPatchChar,
	char *&responseChar,
	char *&broadcastChar);

#endif
