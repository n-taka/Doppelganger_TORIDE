#ifndef SHUTDOWN_CPP
#define SHUTDOWN_CPP

#include "pluginCommon.h"
#include <string>
#include <nlohmann/json.hpp>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayRoomChar, ptrStrArrayRoom);
}

void pluginProcess(
	const char *&configCoreChar,
	const char *&configRoomChar,
	const char *&parameterChar,
	char *&configCorePatchChar,
	char *&configRoomPatchChar,
	char *&responseChar,
	char *&broadcastChar)
{
	// [OUT]
	// configCorePatch = {
	//     "active": false
	// }

	nlohmann::json configCorePatch = nlohmann::json::object();
	configCorePatch["active"] = false;

	// write result
	writeJSONToChar(configCorePatchChar, configCorePatch);
}

#endif
