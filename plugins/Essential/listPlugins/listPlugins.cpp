#ifndef LISTPLUGINS_CPP
#define LISTPLUGINS_CPP

#if defined(_WIN64)
#include <filesystem>
namespace fs = std::filesystem;
#elif defined(__APPLE__)
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;
#elif defined(__linux__)
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include "pluginCommon.h"
#include <string>
#include <nlohmann/json.hpp>

#include "Doppelganger/Util/getPluginCatalogue.h"

#include <iostream>

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
