#ifndef LISTPLUGINS_CPP
#define LISTPLUGINS_CPP

#if defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define DLLEXPORT __attribute__((visibility("default")))
#elif defined(__linux__)
#define DLLEXPORT __attribute__((visibility("default")))
#endif

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

#include <memory>
#include <nlohmann/json.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <fstream>

////
// [IN]
// parameters = {
//  "openDirectory": "plugin"|"output"|"log",
//  "browserAvailability": true|false,
//  "currentConfig": {
//   "server": {
//       "protocol": "http://",
//       "host": "127.0.0.1",
//       "port": 0
//   },
//   "browser": {
//       "type": "chrome",
//       "path": "",
//       "openAs": "app",
//       "openOnStartup": true
//   },
//   "log": {
//       "dir": "",
//       "level": [
//           "SYSTEM",
//           "APICALL",
//           "WSCALL",
//           "ERROR",
//           "MISC",
//           "DEBUG"
//       ],
//       "type": [
//           "STDOUT",
//           "FILE"
//       ]
//   },
//   "output": {
//       "type": "local",
//       "dir": ""
//   },
//   "plugin": {
//       "dir": "",
//       "listURL": [
//           "https://n-taka.info/nextcloud/s/KKH2dWmTRT5wRA5/download/defaultPluginList.json"
//       ]
//   },
//  "update": {
//   "server": {
//       "protocol": "http://",
//       "host": "127.0.0.1",
//       "port": 0
//   },
//   "browser": {
//       "type": "chrome",
//       "path": "",
//       "openAs": "app",
//       "openOnStartup": true
//   },
//   "log": {
//       "dir": "",
//       "level": [
//           "SYSTEM",
//           "APICALL",
//           "WSCALL",
//           "ERROR",
//           "MISC",
//           "DEBUG"
//       ],
//       "type": [
//           "STDOUT",
//           "FILE"
//       ]
//   },
//   "output": {
//       "type": "local",
//       "dir": ""
//   },
//   "plugin": {
//       "dir": "",
//       "listURL": [
//           "https://n-taka.info/nextcloud/s/KKH2dWmTRT5wRA5/download/defaultPluginList.json"
//       ]
//   }
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

namespace
{
	void pluginProcess(
		const char *info,
		const char *parameter,
		char *response,
		char *broadcast)
	{
		const nlohmann::json infoJson = nlohmann::json::parse(info);
		const nlohmann::json parameterJson = nlohmann::json::parse(parameter);
		response = nullptr;
		broadcast = nullptr;

		// create response/broadcast
		nlohmann::json responseJson = nlohmann::json::object();
		nlohmann::json broadcastJson = nlohmann::json::object();

		responseJson = parameterJson.at("currentConfig");
		responseJson.update(parameterJson.at("update"));

		if (parameterJson.contains("openDirectory"))
		{
			// request for open directory
			const std::string &target = parameterJson.at("openDirectory").get<std::string>();
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
				fs::path pluginDir(infoJson.at("pluginDir").get<std::string>());
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
				fs::path outputDir(infoJson.at("dataDir").get<std::string>());
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
				fs::path logDir(infoJson.at("dataDir").get<std::string>());
				logDir.append("log");
				cmd << logDir.string();
				cmd << "\"";
				system(cmd.str().c_str());
			}
		}
		else if (parameterJson.contains("browserAvailability") && parameterJson.at("browserAvailability").get<bool>())
		{
			// request for generateUI
			// browser availability
			responseJson.at("browser")["availableBrowsers"] = nlohmann::json::array();

			if (responseJson.at("server").at("host") == "127.0.0.1")
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
						p.make_preferred();
						if (fs::exists(p))
						{
							responseJson.at("browser").at("availableBrowsers").push_back("chrome");
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
						p.make_preferred();
						if (fs::exists(p))
						{
							responseJson.at("browser").at("availableBrowsers").push_back("firefox");
							break;
						}
					}
				}

				// edge
				{
#if defined(_WIN64)
					fs::path edgePath("C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe");
					edgePath.make_preferred();
					if (fs::exists(edgePath))
					{
						responseJson.at("browser").at("availableBrowsers").push_back("edge");
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
					safariPath.make_preferred();
					if (fs::exists(safariPath))
					{
						responseJson.at("browser").at("availableBrowsers").push_back("safari");
					}
#elif defined(__linux__)
					// nothing
#endif
				}

				// default
				{
					responseJson.at("browser").at("availableBrowsers").push_back("default");
				}
			}
		}
		else
		{

			// write to config.json
			// switch for demo on web (then, we need to update current "room" config)
			{
				std::lock_guard<std::mutex> lock(room->core_->mutexConfig);
				room->core_->updateConfig(response);
			}
		}
	}
}

extern "C" DLLEXPORT void corePluginProcess(
	char *info,
	char *parameter,
	char *response,
	char *broadcast)
{
	const nlohmann::json parameterJson = nlohmann::json::parse(parameter);
	response = nullptr;
	broadcast = nullptr;
}

extern "C" DLLEXPORT void roomPluginProcess(
	char *info,
	char *parameter,
	char *response,
	char *broadcast)
{
}

extern "C" DLLEXPORT void meshPluginProcess(
	const char *info,
	const char *parameter,
	char *response,
	char *broadcast)
{
	// do nothing
	response = nullptr;
	broadcast = nullptr;
}

#endif
