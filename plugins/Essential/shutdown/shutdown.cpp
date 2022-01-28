#ifndef SHUTDOWN_CPP
#define SHUTDOWN_CPP

#include "pluginCommon.h"
#include <string>
#include <nlohmann/json.hpp>

extern "C" DLLEXPORT void pluginProcess(
	const char *&configCoreChar,
	const char *&configRoomChar,
	const char *&parameterChar,
	char *&configCoreUpdateChar,
	char *&configRoomUpdateChar,
	char *&responseChar,
	char *&broadcastChar)
{
	// [OUT]
	// configCoreUpdate = {
	//     "active": false
	// }

	nlohmann::json configCoreUpdate = nlohmann::json::object();
	configCoreUpdate["active"] = false;

	// write result
	writeJSONToChar(configCoreUpdateChar, configCoreUpdate);
}

#endif
