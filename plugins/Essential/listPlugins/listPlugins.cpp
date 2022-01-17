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
		pluginInfo["name"] = plugin->name_;
		pluginInfo["installedVersion"] = plugin->installedVersion;
		pluginInfo["versions"] = plugin->parameters_.at("versions");
		pluginInfo["description"] = plugin->parameters_.at("description");
		pluginInfo["UIPosition"] = plugin->parameters_.at("UIPosition");
		pluginInfo["optional"] = plugin->parameters_.at("optional");
		pluginInfo["hasModuleJS"] = plugin->hasModuleJS;
		return pluginInfo;
	};

	// get current config
	nlohmann::json config;
	{
		std::lock_guard<std::mutex> lock(room->mutexConfig);
		room->getCurrentConfig(config);
	}

	// get plugin catalogue
	nlohmann::json pluginCatalogue;
	room->core_->getPluginCatalogue(config.at("plugin").at("listURL"), pluginCatalogue);

	// initialize Doppelganger::Plugin instances
	for (const auto &pluginEntry : pluginCatalogue.items())
	{
		const std::string &name = pluginEntry.key();
		if (room->plugin.find(name) == room->plugin.end())
		{
			const nlohmann::json &pluginInfo = pluginEntry.value();
			room->plugin[name] = std::make_shared<Doppelganger::Plugin>(room->core_, name, pluginInfo);
		}
	}

	const nlohmann::json installedPluginJson = config.at("plugin").at("installed");

	for (const auto &installedPlugin : installedPluginJson.items())
	{
		const std::string &name = installedPlugin.key();
		if (room->plugin.find(name) != room->plugin.end())
		{
			const std::shared_ptr<Doppelganger::Plugin> &plugin = room->plugin.at(name);
			const nlohmann::json pluginInfo = formatPluginInfo(plugin);
			response.push_back(pluginInfo);
		}
	}

	const std::unordered_map<std::string, std::shared_ptr<Doppelganger::Plugin>> &plugins = room->plugin;
	for (const auto &name_plugin : plugins)
	{
		const std::string &name = name_plugin.first;
		const std::shared_ptr<Doppelganger::Plugin> &plugin = name_plugin.second;
		if (plugin->installedVersion == "")
		{
			const nlohmann::json pluginInfo = formatPluginInfo(plugin);
			response.push_back(pluginInfo);
		}
	}
}

#endif
