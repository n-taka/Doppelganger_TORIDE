#ifndef UPDATEPLUGINS_CPP
#define UPDATEPLUGINS_CPP

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
	////
	// [IN]
	// parameters = [
	//     {
	//         "name": "pluginName-A",
	//         "version": "new version"
	//     },
	//     {
	//         "name": "pluginName-B",
	//         "version": "" (empty string indicates this plugin is not installed)
	//     },
	//     ...
	// ]

	// [OUT]
	// config(Core|Room)Patch = {
	//     "plugin": {
	//         "installed": [
	//             {
	//                 "name": "pluginName-A",
	//                 "version": "new version"
	//             },
	//             {
	//                 "name": "pluginName-B",
	//                 "version": "" (empty string indicates this plugin is not installed)
	//             },
	//             ...
	//         ]
	//     }
	// }

	// initialize
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	// for single user: update Core/Room
	// for multiple user (e.g. web demo): update Room
	nlohmann::json configRoomPatch = nlohmann::json::object();
	configRoomPatch["plugin"] = nlohmann::json::object();
	configRoomPatch.at("plugin")["installed"] = parameter;
	configRoomPatch.at("plugin")["reInstall"] = true;

	// "forceReload" in Core reloads all rooms
	configRoomPatch["forceReload"] = true;

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
}

#endif
