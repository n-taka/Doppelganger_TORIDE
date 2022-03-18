#ifndef LOADTEXTURE_CPP
#define LOADTEXTURE_CPP

#include "Doppelganger/pluginCommon.h"

#include "Doppelganger/Util/filesystem.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/uuid.h"
#include "Doppelganger/Util/writeBase64ToFile.h"
#include "Doppelganger/Util/storeHistory.h"
#include "Doppelganger/Util/log.h"

#include <string>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "Doppelganger/Util/stb_image.h"

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/dataDir");
	ptrStrArrayRoom.push_back("/log");
	ptrStrArrayRoom.push_back("/history");
	{
		std::string targetMeshPath("/meshes/");
		targetMeshPath += parameter.at("meshUUID").get<std::string>();
		ptrStrArrayRoom.push_back(targetMeshPath);
	}
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
	// parameter = {
	// 	"meshUUID": which mesh we try to add texture?
	// 	"texture": {
	// 	 "name": name of this texture (usually, filename without extension),
	// 	 "file": {
	// 	  "type": extension of this file,
	// 	  "base64Str": base64-encoded fragment
	// 	 }
	// 	}
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the loaded mesh
	//  }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json configRoomPatch = nlohmann::json::object();
	nlohmann::json broadcast = nlohmann::json::object();

	const nlohmann::json &fileJson = parameter.at("texture").at("file");
	const std::string fileType = fileJson.at("type").get<std::string>();
	const std::string base64Str = fileJson.at("base64Str").get<std::string>();

	{
		// get mesh
		const std::string meshUUID = parameter.at("meshUUID").get<std::string>();
		Doppelganger::TriangleMesh mesh = configRoom.at("meshes").at(meshUUID).get<Doppelganger::TriangleMesh>();

		// edit history
		nlohmann::json diff = nlohmann::json::object();
		nlohmann::json diffInv = nlohmann::json::object();
		diffInv["meshes"] = nlohmann::json::object();
		diffInv["meshes"][meshUUID] = mesh;

		// write tex to tempfile
		fs::path filePath = fs::temp_directory_path();
		filePath /= Doppelganger::Util::uuid("DoppelgangerTmpFile-");
		filePath += ".";
		filePath += fileType;
		Doppelganger::Util::writeBase64ToFile(base64Str, filePath);

		Doppelganger::TriangleMesh::Texture texture;
		{
			texture.fileName_ = parameter.at("texture").at("name").get<std::string>();
			texture.fileFormat_ = fileType;

			int width, height, channels;
			// first pixel corresponds to the lower left corner
			stbi_set_flip_vertically_on_load(true);
			unsigned char *image = stbi_load(filePath.string().c_str(),
											 &width,
											 &height,
											 &channels,
											 STBI_rgb_alpha);

			texture.texData_.resize(height, width);
			union
			{
				uint32_t uint_;
				unsigned char uchar_[4];
			} u;

			for (int h = 0; h < height; ++h)
			{
				for (int w = 0; w < width; ++w)
				{
					for (int rgba = 0; rgba < STBI_rgb_alpha; ++rgba)
					{
						u.uchar_[rgba] = image[(h * width + w) * STBI_rgb_alpha + rgba];
					}
					texture.texData_(h, w) = u.uint_;
				}
			}
			stbi_image_free(image);
		}
		mesh.textures_.push_back(texture);

		// register to this room
		configRoomPatch["meshes"] = nlohmann::json::object();
		configRoomPatch.at("meshes")[mesh.UUID_] = mesh;

		// write broadcast
		nlohmann::json meshJsonf;
		Doppelganger::to_json(meshJsonf, mesh, true);
		broadcast["meshes"] = nlohmann::json::object();
		broadcast.at("meshes")[mesh.UUID_] = meshJsonf;

		{
			// message
			std::stringstream ss;
			ss << "New texture for \"" << mesh.UUID_ << "\" is loaded.";
			Doppelganger::Util::log(ss.str(), "APICALL", configRoom);
			// file
			Doppelganger::Util::log(filePath, "APICALL", configRoom);
		}

		// edit history
		{
			diff["meshes"] = nlohmann::json::object();
			diff["meshes"][meshUUID] = configRoomPatch.at("meshes").at(mesh.UUID_);

			configRoomPatch["history"] = nlohmann::json::object();
			Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));
		}

		// write result
		writeJSONToChar(configRoomPatchChar, configRoomPatch);
		writeJSONToChar(broadcastChar, broadcast);
	}
}

#endif
