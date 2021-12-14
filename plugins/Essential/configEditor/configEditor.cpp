#ifndef LISTPLUGINS_CPP
#define LISTPLUGINS_CPP

#if defined(_WIN32) || defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __attribute__((visibility("default")))
#endif
#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Room.h"
#include "Doppelganger/Core.h"

#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;
#endif

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
{
	////
	// [IN]
	// parameters = {
	//  "forUIGeneration": true|false,
	//  "openDirectory": "plugin"|"output"|"log",
	//  "server": {
	//      "protocol": "http://",
	//      "host": "127.0.0.1",
	//      "port": 0
	//  },
	//  "browser": {
	//      "type": "chrome",
	//      "path": "",
	//      "openAs": "app",
	//      "openOnStartup": true
	//  },
	//  "log": {
	//      "dir": "",
	//      "level": [
	//          "SYSTEM",
	//          "APICALL",
	//          "WSCALL",
	//          "ERROR",
	//          "MISC",
	//          "DEBUG"
	//      ],
	//      "type": [
	//          "STDOUT",
	//          "FILE"
	//      ]
	//  },
	//  "output": {
	//      "type": "local",
	//      "dir": ""
	//  },
	//  "plugin": {
	//      "dir": "",
	//      "listURL": [
	//          "https://n-taka.info/nextcloud/s/KKH2dWmTRT5wRA5/download/defaultPluginList.json"
	//      ]
	//  }
	// }

	// [OUT]
	// response = {
	//  current JSON parameters (with browser availability) when forUIGeneration == true
	// }
	// response = {
	//  updated JSON parameters when forUIGeneration == false
	// }
	// broadcast = {
	// }

	// create response/broadcast
	response = nlohmann::json::object();
	broadcast = nlohmann::json::object();

	// current settings
	response = room->core->configFileContent;

	if (parameters.contains("openDirectory"))
	{
		// request for open directory
		const std::string &target = parameters.at("openDirectory").get<std::string>();
		if (target == "plugin")
		{
			std::stringstream cmd;
#if defined(_WIN32) || defined(_WIN64)
			cmd << "start \"\" \"";
#elif defined(__APPLE__)
			cmd << "open \"";
#endif
			fs::path pluginDir = room->core->systemParams.workingDir;
			pluginDir.append("plugin");
			cmd << pluginDir.string();
			cmd << "\"";
			system(cmd.str().c_str());
		}
		else if (target == "output")
		{
			std::stringstream cmd;
#if defined(_WIN32) || defined(_WIN64)
			cmd << "start \"\" \"";
#elif defined(__APPLE__)
			cmd << "open \"";
#endif
			cmd << room->outputDir.string();
			cmd << "\"";
			system(cmd.str().c_str());
		}
		else if (target == "log")
		{
			std::stringstream cmd;
#if defined(_WIN32) || defined(_WIN64)
			cmd << "start \"\" \"";
#elif defined(__APPLE__)
			cmd << "open \"";
#endif
			cmd << room->logger.logDir.string();
			cmd << "\"";
			system(cmd.str().c_str());
		}
	}
	else if (parameters.contains("forUIGeneration") && parameters.at("forUIGeneration").get<bool>())
	{
		// request for generateUI
		// browser availability
		response.at("browser")["availableBrowsers"] = nlohmann::json::array();

		// chrome
		{
#if defined(_WIN32) || defined(_WIN64)
			std::vector<fs::path> chromePaths({fs::path("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"),
											   fs::path("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe")});
#elif defined(__APPLE__)
			std::vector<fs::path> chromePaths({fs::path("/Applications/Google Chrome.app/Contents/MacOS/Google Chrome")});
#endif
			for (auto &p : chromePaths)
			{
				p.make_preferred();
				if (fs::exists(p))
				{
					response.at("browser").at("availableBrowsers").push_back("chrome");
					break;
				}
			}
		}

		// firefox
		{
#if defined(_WIN32) || defined(_WIN64)
			std::vector<fs::path> firefoxPaths({fs::path("C:\\Program Files\\Mozilla Firefox\\firefox.exe"),
												fs::path("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe")});
#elif defined(__APPLE__)
			// todo update
			std::vector<fs::path> firefoxPaths({fs::path("/Applications/Firefox.app/Contents/MacOS/firefox")});
#endif
			for (auto &p : firefoxPaths)
			{
				p.make_preferred();
				if (fs::exists(p))
				{
					response.at("browser").at("availableBrowsers").push_back("firefox");
					break;
				}
			}
		}

		// edge
		{
#if defined(_WIN32) || defined(_WIN64)
			fs::path edgePath("C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe");
			edgePath.make_preferred();
			if (fs::exists(edgePath))
			{
				response.at("browser").at("availableBrowsers").push_back("edge");
			}
#elif defined(__APPLE__)
			// nothing
#endif
		}

		// safari
		{
#if defined(_WIN32) || defined(_WIN64)
			// nothing
#elif defined(__APPLE__)
			// todo update
			fs::path safariPath("/Applications/Safari.app/Contents/MacOS/safari");
			safariPath.make_preferred();
			if (fs::exists(safariPath))
			{
				response.at("browser").at("availableBrowsers").push_back("safari");
			}
#endif
		}

		// default
		{
			response.at("browser").at("availableBrowsers").push_back("default");
		}
	}
	else
	{
		// request for update

		// current config always has 2-depth, we can easily implement as follows;
		for (const auto &parameterItem : parameters.items())
		{
			// add parameter if is doesn't exist
			if (!response.contains(parameterItem.key()))
			{
				response[parameterItem.key()] = nlohmann::json::object();
			}

			const nlohmann::json &parametersJson = parameters.at(parameterItem.key());
			nlohmann::json &responseJson = response.at(parameterItem.key());
			for (const auto &item : parametersJson.items())
			{
				responseJson[item.key()] = item.value();
			}
		}

		// write to config.json
		//   for human readability, we use .dump(4);
		{
			std::lock_guard<std::mutex> lock(room->core->systemParams.mutex);
			fs::path configPath(room->core->systemParams.workingDir);
			configPath.append("config.json");
			std::ofstream ofs(configPath.string());
			ofs << response.dump(4);
			ofs.close();
		}
	}
}

#endif
