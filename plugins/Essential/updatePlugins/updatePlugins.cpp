#ifndef UPDATEPLUGINS_CPP
#define UPDATEPLUGINS_CPP

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
#include "Doppelganger/Logger.h"

#include <string>
#include <fstream>

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
{
	////
	// [IN]
	// parameters = [
	//  {
	//   "name": "pluginName-A",
	//   "version": "new version"
	//  },
	//  {
	//   "name": "pluginName-B",
	//   "version": "" (empty string indicates this plugin is not installed)
	//  },
	//  ...
	// ]

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// }

	// create response/broadcast
	response = nlohmann::json::object();
	broadcast = nlohmann::json::object();

	// switch for demo on web (then, we don't update for core)
	{
		nlohmann::json config;
		std::lock_guard<std::mutex> lock(room->mutexConfig);
		room->getCurrentConfig(config);
		config.at("plugin").at("installed") = nlohmann::json::object();
		room->updateConfig(config);
	}
	{
		nlohmann::json config;
		std::lock_guard<std::mutex> lock(room->core_->mutexConfig);
		room->core_->getCurrentConfig(config);
		config.at("plugin").at("installed") = nlohmann::json::object();
		room->core_->updateConfig(config);
	}

	// validate the parameters
	for (const auto &entry : parameters)
	{
		const std::string &name = entry.at("name").get<std::string>();
		const std::string &version = entry.at("version").get<std::string>();

		if (room->plugin.find(name) != room->plugin.end())
		{
			const std::shared_ptr<Doppelganger::Plugin> &plugin = room->plugin.at(name);
			if (version.size() > 0)
			{
				const nlohmann::json &pluginParameters = plugin->parameters_;
				const std::string actualVersion((version == "latest") ? pluginParameters.at("versions").at(0).at("version").get<std::string>() : version);

				bool versionFound = false;
				for (const auto &versionEntry : pluginParameters.at("versions"))
				{
					const std::string &pluginVersion = versionEntry.at("version").get<std::string>();
					if (pluginVersion == actualVersion)
					{
						// switch for demo on web (then, we need to use false)
						plugin->install(room, version, true);

						versionFound = true;
						break;
					}
				}
				if (!versionFound)
				{
					std::stringstream ss;
					ss << "Plugin \"" << name << "\" invalid version \"";
					if (version == "latest")
					{
						ss << "latest, ";
					}
					ss << actualVersion << "\" is specified.";
					room->logger.log(ss.str(), "ERROR");
				}
			}
		}
		else
		{
			std::stringstream ss;
			ss << "Unknown plugin \"" << name << "\" is specified.";
			room->logger.log(ss.str(), "ERROR");
		}
	}
}

#endif
