#ifndef TOGGLEMESHVISIBILITY_CPP
#define TOGGLEMESHVISIBILITY_CPP

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
	for (const auto &UUID : parameter.at("meshes"))
	{
		std::string targetMeshPath("/meshes/");
		targetMeshPath += UUID.get<std::string>();
		targetMeshPath += "/visibility";
		ptrStrArrayRoom.push_back(targetMeshPath);
	}
	ptrStrArrayRoom.push_back("/history");
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
	// 	"meshes": [
	// 	 "UUID-A",
	// 	 "UUID-B",
	//   ...
	//  ]
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
	nlohmann::json diff = nlohmann::json::object();
	nlohmann::json diffInv = nlohmann::json::object();

	broadcast["meshes"] = nlohmann::json::object();
	diff["meshes"] = nlohmann::json::object();
	diffInv["meshes"] = nlohmann::json::object();

	// toggle visibility
	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		nlohmann::json visibilityJson = nlohmann::json::object();
		visibilityJson["visibility"] = configRoom.at("meshes").at(meshUUID).at("visibility");

		diffInv.at("meshes")[meshUUID] = visibilityJson;
		visibilityJson["visibility"] = !visibilityJson["visibility"].get<bool>();
		diff.at("meshes")[meshUUID] = visibilityJson;

		broadcast.at("meshes")[meshUUID] = visibilityJson;
	}
	configRoomPatch = diff;

	// history
	configRoomPatch["history"] = nlohmann::json::object();
	Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
	writeJSONToChar(broadcastChar, broadcast);
}

#endif
