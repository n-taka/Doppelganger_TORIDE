#ifndef CONFIGEDITOR_CPP
#define CONFIGEDITOR_CPP

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
	////
	// [IN]
	// parameters = {
	//     "openDirectory": "plugin"|"output"|"log",
	//     "forUIGeneration": true|false,
	//     "server": {
	//         "protocol": "http",
	//         "host": "127.0.0.1",
	//         "port": 0
	//     },
	//     "browser": {
	//         "type": "chrome",
	//         "openMode": "app",
	//         "openOnStartup": true
	//     },
	//     "log": {
	//         "level": [
	//             "SYSTEM",
	//             "APICALL",
	//             "WSCALL",
	//             "ERROR",
	//             "MISC",
	//             "DEBUG"
	//         ],
	//         "type": [
	//             "STDOUT",
	//             "FILE"
	//         ]
	//     },
	//     "output": {
	//         "type": "local",
	//     },
	//     "plugin": {
	//         "listURL": [
	//             "https://n-taka.info/nextcloud/s/KKH2dWmTRT5wRA5/download/defaultPluginList.json"
	//         ]
	//     }
	// }

	// [OUT]
	// case 1. forUIGeneration == true
	// response = {
	//     current JSON parameters (with browser availability)
	// }
	// case 2. openDirectory == "log"|"output"|"plugin"
	// response = {
	//     <open corresponding directory>
	// }
	// case 3. otherwise
	// config(Core|Room)Update = {
	//     updated JSON parameters
	// }

	// initialize
	const nlohmann::json configCore = nlohmann::json::parse(configCoreChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	// for single user: update Core/Room
	// for multiple user (e.g. web demo): update Room

	if (parameter.contains("forUIGeneration") && parameter.at("forUIGeneration").get<bool>())
	{
		// case 1.
		// request for generateUI

		nlohmann::json response = configCore;
		// browser availability
		response.at("browser")["availableBrowsers"] = nlohmann::json::array();
		// we allow browser option only when the program runs on localhost
		if (response.at("server").at("host") == "127.0.0.1")
		{
			// chrome
			{
#if defined(_WIN64)
				std::vector<fs::path> chromePaths({fs::path("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"),
												   fs::path("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe")});
#elif defined(__APPLE__)
				std::vector<fs::path> chromePaths({fs::path("/Applications/Google Chrome.app/Contents/MacOS/Google Chrome")});
#elif defined(__linux__)
				std::vector<fs::path> chromePaths({fs::path("/opt/google/chrome/google-chrome")});
#endif
				for (auto &p : chromePaths)
				{
					if (fs::exists(p))
					{
						response.at("browser").at("availableBrowsers").push_back("chrome");
						break;
					}
				}
			}

			// firefox
			{
#if defined(_WIN64)
				std::vector<fs::path> firefoxPaths({fs::path("C:\\Program Files\\Mozilla Firefox\\firefox.exe"),
													fs::path("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe")});
#elif defined(__APPLE__)
				std::vector<fs::path> firefoxPaths({fs::path("/Applications/Firefox.app/Contents/MacOS/firefox")});
#elif defined(__linux__)
				std::vector<fs::path> firefoxPaths({fs::path("/usr/bin/firefox")});
#endif
				for (auto &p : firefoxPaths)
				{
					if (fs::exists(p))
					{
						response.at("browser").at("availableBrowsers").push_back("firefox");
						break;
					}
				}
			}

			// edge
			{
#if defined(_WIN64)
				fs::path edgePath("C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe");
				if (fs::exists(edgePath))
				{
					response.at("browser").at("availableBrowsers").push_back("edge");
				}
#elif defined(__APPLE__)
				// nothing
#elif defined(__linux__)
				// nothing
#endif
			}

			// safari
			{
#if defined(_WIN64)
				// nothing
#elif defined(__APPLE__)
				// todo update
				fs::path safariPath("/Applications/Safari.app/Contents/MacOS/safari");
				if (fs::exists(safariPath))
				{
					response.at("browser").at("availableBrowsers").push_back("safari");
				}
#elif defined(__linux__)
				// nothing
#endif
			}

			// default
			{
				response.at("browser").at("availableBrowsers").push_back("default");
			}
		}

		// write result
		writeJSONToChar(responseChar, response);
	}
	else if (parameter.contains("openDirectory"))
	{
		// case 2.
		// we allow openDirectory only when the program runs on localhost
		if (configCore.at("server").at("host") == "127.0.0.1")
		{
			// request for open directory
			const std::string &target = parameter.at("openDirectory").get<std::string>();
			if (target == "plugin")
			{
				std::stringstream cmd;
#if defined(_WIN64)
				cmd << "start \"\" \"";
#elif defined(__APPLE__)
				cmd << "open \"";
#elif defined(__linux__)
				cmd << "xdg-open \"";
#endif
				fs::path pluginDir(configCore.at("doppelgangerRootDir").get<std::string>());
				pluginDir.append("plugin");
				cmd << pluginDir.string();
				cmd << "\"";
				system(cmd.str().c_str());
			}
			else if (target == "output")
			{
				std::stringstream cmd;
#if defined(_WIN64)
				cmd << "start \"\" \"";
#elif defined(__APPLE__)
				cmd << "open \"";
#elif defined(__linux__)
				cmd << "xdg-open \"";
#endif
				fs::path outputDir(configCore.at("dataDir").get<std::string>());
				outputDir.append("output");
				cmd << outputDir.string();
				cmd << "\"";
				system(cmd.str().c_str());
			}
			else if (target == "log")
			{
				std::stringstream cmd;
#if defined(_WIN64)
				cmd << "start \"\" \"";
#elif defined(__APPLE__)
				cmd << "open \"";
#elif defined(__linux__)
				cmd << "xdg-open \"";
#endif
				fs::path logDir(configCore.at("dataDir").get<std::string>());
				logDir.append("log");
				cmd << logDir.string();
				cmd << "\"";
				system(cmd.str().c_str());
			}
		}
	}
	else
	{
		// case 3. otherwise
		writeJSONToChar(configCoreUpdateChar, parameter);
	}
}

#endif
