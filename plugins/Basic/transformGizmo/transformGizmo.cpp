#ifndef TRANSFORMGIZMO_CPP
#define TRANSFORMGIZMO_CPP

#include "Doppelganger/pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/storeHistory.h"

#include <string>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	if (parameter.at("storeHistory").get<bool>())
	{
		// HTTP API call just for storing history requires edit history
		ptrStrArrayRoom.push_back("/history");
	}

	for (const auto &uuid_value : parameter.at("meshes").items())
	{
		const std::string &UUID = uuid_value.key();
		std::string targetMeshPath("/meshes/");
		targetMeshPath += UUID;
		targetMeshPath += "/matrix/world";
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
	// parameters = {
	// 	"meshes": {
	// 	 "UUID-A" : {
	//	  "matrix": {
	//     "world": [column-major 16 elements]
	//    }
	//   },
	//   ...
	//  }
	// }

	// [OUT]
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

	if (parameter.at("storeHistory").get<bool>())
	{
		// store history
		nlohmann::json diff = nlohmann::json::object();
		nlohmann::json diffInv = nlohmann::json::object();

		diffInv = parameter.at("beforeTransform");
		diff["meshes"] = parameter.at("meshes");

		// history
		configRoomPatch["history"] = nlohmann::json::object();
		Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));

		writeJSONToChar(configRoomPatchChar, configRoomPatch);
	}
	else
	{
		// update transformation
		broadcast["meshes"] = parameter.at("meshes");
		configRoomPatch["meshes"] = parameter.at("meshes");

		// write result
		writeJSONToChar(configRoomPatchChar, configRoomPatch);
		writeJSONToChar(broadcastChar, broadcast);
	}
}

#endif
