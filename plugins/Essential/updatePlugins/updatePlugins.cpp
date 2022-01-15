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
	//   "dest": "core"|"room",
	//   "version": "new version"
	//  },
	//  {
	//   "name": "pluginName-B",
	//   "dest": "core"|"room",
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
	nlohmann::json installedArrayCore = nlohmann::json::array();
	nlohmann::json installedArrayRoom = nlohmann::json::array();
	for (const auto &entry : parameters)
	{
		const std::string &name = entry.at("name").get<std::string>();
		const std::string &dest = entry.at("dest").get<std::string>();
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
						if (dest == "core")
						{
							installedArrayCore.push_back(entry);
							plugin->install(room->core_, actualVersion);
						}
						else
						{
							installedArrayRoom.push_back(entry);
							plugin->install(room, actualVersion);
						}

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

	// core
	{
		fs::path installedPluginJsonPath(room->core_->DoppelgangerRootDir);
		installedPluginJsonPath.append("plugin");
		installedPluginJsonPath.append("installed.json");
		std::ofstream ofs(installedPluginJsonPath.string());
		ofs << installedArrayCore.dump(4);
		ofs.close();
	}

	// room
	{
		fs::path installedPluginJsonPath(room->dataDir);
		installedPluginJsonPath.append("installed.json");
		std::ofstream ofs(installedPluginJsonPath.string());
		ofs << installedArrayRoom.dump(4);
		ofs.close();
	}
}

#endif
