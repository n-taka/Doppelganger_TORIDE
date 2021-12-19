#ifndef LISTPLUGINS_CPP
#define LISTPLUGINS_CPP

#if defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define DLLEXPORT __attribute__((visibility("default")))
#elif defined(__linux__)
#define DLLEXPORT __attribute__((visibility("default")))
#endif

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Core.h"
#include "Doppelganger/Room.h"
#include "Doppelganger/Plugin.h"

#include <string>
#include <fstream>

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
{
	////
	// [IN]
	// parameters = {
	// }

	// [OUT]
	// response = [
	//  {
	//   "name": "pluginNameA",
	//   "installedVersion": "" (if text is zero-length, which means not installed),
	//   "versions": ["1.0.2", "1.0.1", "1.0.0"],
	//   "description": {
	//    "en": "description text for this plugin in en",
	//    "ja": "description text for this plugin in ja",
	//    ...
	//   },
	//   "UIPosition": "topLeft"|"topRight"|"mesh"|"bottomSummary"|"bottomLeft"|"bottomRight",
	//   "optional": true|false,
	//   "hasModuleJS": true|false
	//  },
	//  {
	//   "name": "pluginNameB",
	//   "installedVersion": "" (if text is zero-length, which means not installed),
	//   "versions": ["1.0.2", "1.0.1", "1.0.0"],
	//   "description": {
	//    "en": "description text for this plugin in en",
	//    "ja": "description text for this plugin in ja",
	//    ...
	//   },
	//   "UIPosition": "topLeft"|"topRight"|"mesh"|"bottomSummary"|"bottomLeft"|"bottomRight",
	//   "optional": true|false,
	//   "hasModuleJS": true|false
	//  },
	//  ...
	// ]
	// broadcast = {
	// }

	// create response/broadcast
	response = nlohmann::json::array();
	broadcast = nlohmann::json::object();

	const auto formatPluginInfo = [](const std::shared_ptr<Doppelganger::Plugin> &plugin)
	{
		nlohmann::json pluginInfo = nlohmann::json::object();
		pluginInfo["name"] = plugin->name;
		pluginInfo["installedVersion"] = plugin->installedVersion;
		pluginInfo["versions"] = plugin->parameters.at("versions");
		pluginInfo["description"] = plugin->parameters.at("description");
		pluginInfo["UIPosition"] = plugin->parameters.at("UIPosition");
		pluginInfo["optional"] = plugin->parameters.at("optional");
		pluginInfo["hasModuleJS"] = plugin->hasModuleJS;
		return pluginInfo;
	};

	// first we push_back installed plugins to the response array
	fs::path installedPluginJsonPath(room->core->config.at("plugin").at("dir").get<std::string>());
	installedPluginJsonPath.append("installed.json");
	std::ifstream ifs(installedPluginJsonPath.string());
	const nlohmann::json installedPluginJson = nlohmann::json::parse(ifs);
	ifs.close();

	for (const auto &installedPlugin : installedPluginJson)
	{
		const std::string &name = installedPlugin.at("name").get<std::string>();
		const std::shared_ptr<Doppelganger::Plugin> &plugin = room->core->plugin.at(name);
		const nlohmann::json pluginInfo = formatPluginInfo(plugin);
		response.push_back(pluginInfo);
	}

	const std::unordered_map<std::string, std::shared_ptr<Doppelganger::Plugin> > &plugins = room->core->plugin;
	for (const auto &name_plugin : plugins)
	{
		const std::string &name = name_plugin.first;
		const std::shared_ptr<Doppelganger::Plugin> plugin = name_plugin.second;
		// here, we only deal with not installed plugins
		if (plugin->installedVersion == "")
		{
			const nlohmann::json pluginInfo = formatPluginInfo(plugin);
			response.push_back(pluginInfo);
		}
	}
}

#endif