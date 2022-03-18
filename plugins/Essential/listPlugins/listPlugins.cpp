#ifndef LISTPLUGINS_CPP
#define LISTPLUGINS_CPP

#include "Doppelganger/Util/filesystem.h"

#include "Doppelganger/pluginCommon.h"
#include <string>
#include <nlohmann/json.hpp>
#include "Doppelganger/Util/getPluginCatalogue.h"

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/DoppelgangerRootDir");
	ptrStrArrayRoom.push_back("/plugin");
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
	// response = [
	//     {
	//         "name": "pluginNameA",
	//         "installedVersion": "" (if text is zero-length, which means not installed),
	//         "versions": [
	//             {
	//                 "version": "1.0.2",
	//                 "URL": "https://..."
	//             },
	//             ...
	//         ],
	//         "description": {
	//             "en": "description text for this plugin in en",
	//             "ja": "description text for this plugin in ja",
	//             ...
	//         },
	//         "UIPosition": "topLeft"|"topRight"|"mesh"|"bottomSummary"|"bottomLeft"|"bottomRight",
	//         "optional": true|false
	//     },
	//     ...
	// ]

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	nlohmann::json response = nlohmann::json::array();

	// get plugin catalogue
	nlohmann::json catalogue;
	{
		fs::path pluginDir(configRoom.at("DoppelgangerRootDir").get<std::string>());
		pluginDir.append("plugin");
		std::vector<std::string> listURLList;
		for (const auto &listURL : configRoom.at("plugin").at("listURL"))
		{
			listURLList.push_back(listURL.get<std::string>());
		}
		Doppelganger::Util::getPluginCatalogue(pluginDir, listURLList, catalogue);
	}

	std::unordered_map<std::string, nlohmann::json> plugins;
	for (const auto &pluginEntry : catalogue)
	{
		const std::string name = pluginEntry.at("name").get<std::string>();
		plugins[name] = pluginEntry;
	}

	for (const auto &pluginInfo : configRoom.at("plugin").at("installed"))
	{
		const std::string name = pluginInfo.at("name").get<std::string>();
		const std::string version = pluginInfo.at("version").get<std::string>();

		// we manually put installedVersion for listing not-installed plugins
		plugins.at(name)["installedVersion"] = version;
		response.push_back(plugins.at(name));
	}

	for (const auto &name_plugin : plugins)
	{
		const std::string &name = name_plugin.first;
		const nlohmann::json &plugin = name_plugin.second;
		if (!plugin.contains("installedVersion"))
		{
			response.push_back(plugin);
		}
	}

	// write result
	writeJSONToChar(responseChar, response);
}

#endif
