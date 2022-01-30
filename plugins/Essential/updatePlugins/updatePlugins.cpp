#ifndef UPDATEPLUGINS_CPP
#define UPDATEPLUGINS_CPP

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
	// config(Core|Room)Update = {
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
	nlohmann::json configCoreUpdate = nlohmann::json::object();
	nlohmann::json configRoomUpdate = nlohmann::json::object();

	configCoreUpdate["plugin"] = nlohmann::json::object();
	configCoreUpdate.at("plugin")["installed"] = parameter;
	configRoomUpdate["plugin"] = nlohmann::json::object();
	configRoomUpdate.at("plugin")["installed"] = parameter;

	// write result
	writeJSONToChar(configRoomUpdateChar, configRoomUpdate);
	// "forceReload" in Core reloads all rooms
	configCoreUpdate["forceReload"] = true;
	writeJSONToChar(configCoreUpdateChar, configCoreUpdate);
}

#endif
