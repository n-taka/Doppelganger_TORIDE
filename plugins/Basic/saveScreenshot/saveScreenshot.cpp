#ifndef SAVESCREENSHOT_CPP
#define SAVESCREENSHOT_CPP

#include "pluginCommon.h"

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

#include "Doppelganger/Util/writeBase64ToFile.h"

#include <string>
#include <fstream>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/output");
	ptrStrArrayRoom.push_back("/dataDir");
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
	////
	// [IN]
	// parameters = {
	// 	   "screenshot": {
	// 	       "name": name of this screenshot (usually, filename without extension),
	// 	       "file": {
	// 	           "type": extensiton of this file,
	// 	           "base64Str": base64-encoded fragment
	// 	       }
	// 	   }
	// }

	// [OUT]
	// <when configRoom.at("output").at("type") == "download">
	// response = {
	// 	   "screenshots" : [
	// 	       {
	//             "fileName": fileName,
	//             "base64Str": base64-encoded data,
	//             "format": "jpeg"|"png"
	//         },
	//         ...
	//     ]
	// }
	// <when configRoom.at("output").at("type") == "storage">
	// response = {
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json response = nlohmann::json::object();

	const bool saveToLocal = (configRoom.at("output").at("type").get<std::string>() == "storage");

	if (!saveToLocal)
	{
		response["screenshots"] = nlohmann::json::array();
	}

	///////

	const std::string fileName = parameter.at("screenshot").at("name").get<std::string>();
	const std::string fileType = parameter.at("screenshot").at("file").at("type").get<std::string>();
	const std::string base64Str = parameter.at("screenshot").at("file").at("base64Str").get<std::string>();

	if (saveToLocal)
	{
		fs::path directoryPath(configRoom.at("dataDir").get<std::string>());
		directoryPath.append("output");
		// path to screenshot file
		fs::path screenshotFilePath;
		{
			std::string screenshotFileName(fileName);
			screenshotFileName += ".";
			screenshotFileName += fileType;
			screenshotFilePath = directoryPath;
			screenshotFilePath.append(screenshotFileName);
		}
		Doppelganger::Util::writeBase64ToFile(base64Str, screenshotFilePath);

		// open a directory that containing filePath
		// open output directory.
		std::stringstream cmd;
#if defined(_WIN64)
		cmd << "start \"\" \"";
#elif defined(__APPLE__)
		cmd << "open \"";
#elif defined(__linux__)
		cmd << "xdg-open \"";
#endif
		cmd << directoryPath.string();
		cmd << "\"";
		system(cmd.str().c_str());
	}
	else
	{
		nlohmann::json screenshotJson = nlohmann::json::object();
		screenshotJson["fileName"] = fileName;
		screenshotJson["format"] = fileType;
		screenshotJson["base64Str"] = base64Str;
		response.at("screenshots").push_back(screenshotJson);
	}

	if (!saveToLocal)
	{
		// write result
		writeJSONToChar(responseChar, response);
	}
}

#endif
