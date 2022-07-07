#ifndef SHUTDOWN_CPP
#define SHUTDOWN_CPP

#include "Doppelganger/pluginCommon.h"
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
	// }
	// configRoomPatch = {
	//     "active": false
	// }

	// we need empty (not null) JSON object for graceful shutdown (to fire Core::applyCurrentConfig())
	nlohmann::json configCorePatch = nlohmann::json::object();
	nlohmann::json configRoomPatch = nlohmann::json::object();
	configRoomPatch["active"] = false;

	// write result
	writeJSONToChar(configCorePatchChar, configCorePatch);
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
}

#endif
