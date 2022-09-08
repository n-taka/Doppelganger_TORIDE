#ifndef SAVESCREENSHOTALL_CPP
#define SAVESCREENSHOTALL_CPP

#include "Doppelganger/pluginCommon.h"

#include "Doppelganger/Util/filesystem.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Util/writeBase64ToFile.h"
#include "convertImage.h"

#include <string>
#include <sstream>
#include <fstream>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/output");
	ptrStrArrayRoom.push_back("/log");
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
	// 	   "screenshots": {
	// 	       "name": name of the psd file to be saved,
	//              // only valid iff (saveFileFormat == "psd" && mageAsLayers == true)
	//         "saveFileFormat": extension to be downloaded,
	//         "imageAsLayers": true|false,
	// 	       "images": [ // this sequence of images are somehow sorted (usually, based on images, far to close)
	// 	           {
	// 	               "name": name of this image, // usually, mesh name without extension
	// 	               "format": extensiton of this image,
	// 	               "base64Str": base64-encoded fragment
	// 	           },
	// 	           {
	// 	               "name": name of this image, // usually, mesh name without extension
	// 	               "format": extensiton of this image,
	// 	               "base64Str": base64-encoded fragment
	// 	           },
	//             ...
	// 	       ]
	// 	   }
	// }

	// [OUT]
	// <when configRoom.at("output").at("type") == "download">
	// response = {
	// 	   "screenshots" : [
	// 	       {
	//             "fileName": fileName,
	//             "format": "jpeg"|"png",
	//             "base64Str": base64-encoded data
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

	const bool imageAsLayers = parameter.at("screenshots").at("imageAsLayers").get<bool>();
	const std::string formatToBeSaved = parameter.at("screenshots").at("saveFileFormat").get<std::string>();

	std::vector<Image> images;
	for (const auto &image : parameter.at("screenshots").at("images"))
	{
		Image img;
		img.name = image.at("name").get<std::string>();
		img.format = image.at("format").get<std::string>();
		img.fileBase64Str = image.at("base64Str").get<std::string>();
		images.push_back(img);
	}

	const std::string screenshotFileName = parameter.at("screenshots").at("name").get<std::string>();
	std::vector<Image> convertedImages;
	convertImage(images, formatToBeSaved, imageAsLayers, screenshotFileName, configRoom, convertedImages);

	for (const auto &cImage : convertedImages)
	{
		if (saveToLocal)
		{
			fs::path directoryPath(configRoom.at("dataDir").get<std::string>());
			directoryPath.append("output");
			// path to screenshot file
			fs::path screenshotFilePath;
			{
				std::string screenshotFileName(cImage.name);
				screenshotFileName += ".";
				screenshotFileName += cImage.format;
				screenshotFilePath = directoryPath;
				screenshotFilePath.append(screenshotFileName);
			}
			Doppelganger::Util::writeBase64ToFile(cImage.fileBase64Str, screenshotFilePath);
		}
		else
		{
			nlohmann::json screenshotJson = nlohmann::json::object();
			screenshotJson["fileName"] = cImage.name;
			screenshotJson["format"] = cImage.format;
			screenshotJson["base64Str"] = cImage.fileBase64Str;
			response.at("screenshots").push_back(screenshotJson);
		}
	}

	if (saveToLocal)
	{
		fs::path directoryPath(configRoom.at("dataDir").get<std::string>());
		directoryPath.append("output");

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
		// write result
		writeJSONToChar(responseChar, response);
	}
}

#endif
