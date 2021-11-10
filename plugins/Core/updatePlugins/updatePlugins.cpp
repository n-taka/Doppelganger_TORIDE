#ifndef UPDATEPLUGINS_CPP
#define UPDATEPLUGINS_CPP

#if defined(_WIN32) || defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#else
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

	// this plugin updates installed.json
	// actual update is performed on the next boot

	// validate the parameters
	nlohmann::json installedArray = nlohmann::json::array();
	for (const auto &entry : parameters)
	{
		const std::string &name = entry.at("name").get<std::string>();
		const std::string &version = entry.at("version").get<std::string>();

		if (room->core->plugin.find(name) != room->core->plugin.end())
		{
			const std::shared_ptr<Doppelganger::Plugin> &plugin = room->core->plugin.at(name);
			if (version.size() > 0)
			{
				const nlohmann::json &pluginParameters = plugin->parameters;
				const std::string actualVersion((version == "latest") ? pluginParameters.at("versions").at(0).at("version").get<std::string>() : version);

				bool versionFound = false;
				for (const auto &versionEntry : pluginParameters.at("versions"))
				{
					const std::string &pluginVersion = versionEntry.at("version").get<std::string>();
					if (pluginVersion == actualVersion)
					{
						installedArray.push_back(entry);
						versionFound = true;
						break;
					}
				}
				if (!versionFound)
				{
					std::stringstream ss;
					ss << "Plugin \"";
					ss << name;
					ss << "\" invalid version \"";
					if (version == "latest")
					{
						ss << "latest, ";
					}
					ss << actualVersion;
					ss << "\" is specified.";
					room->core->logger.log(ss.str(), "ERROR");
				}
			}
		}
		else
		{
			std::stringstream ss;
			ss << "Unknown plugin \"";
			ss << name;
			ss << "\" is specified.";
			room->core->logger.log(ss.str(), "ERROR");
		}
	}

	fs::path installedPluginJsonPath(room->core->config.at("plugin").at("dir").get<std::string>());
	installedPluginJsonPath.append("installed.json");

	// write to file
	std::ofstream ofs(installedPluginJsonPath.string());
	ofs << installedArray.dump(4);
	ofs.close();
}

#endif
