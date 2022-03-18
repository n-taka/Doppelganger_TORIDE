#ifndef SYNCCURSOR_CPP
#define SYNCCURSOR_CPP

#include "Doppelganger/pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	if (parameter.empty())
	{
		// currentCursors (part of session initialization)
		ptrStrArrayRoom.push_back("/extension/syncCursor");
	}
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
	////
	// [IN]
	// parameters = {
	//     "sessionUUID": {
	//         "dir": {
	//             "x": x corrdinate of this cursor,
	//             "y": y corrdinate of this cursor
	//         },
	//         "icon": idx for cursor icon
	//     }
	// }

	// [OUT]
	// broadcast = parameters

	// initialize
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	if (parameter.empty())
	{
		const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);

		if (configRoom.contains("extension") && configRoom.at("extension").contains("syncCursor"))
		{
			// write result
			writeJSONToChar(broadcastChar, configRoom.at("extension").at("syncCursor"));
		}
	}
	else
	{
		nlohmann::json configRoomPatch = nlohmann::json::object();
		configRoomPatch["extension"] = nlohmann::json::object();
		configRoomPatch.at("extension")["syncCursor"] = parameter;

		// write result
		writeJSONToChar(configRoomPatchChar, configRoomPatch);
		writeJSONToChar(broadcastChar, parameter);
	}
}

#endif
