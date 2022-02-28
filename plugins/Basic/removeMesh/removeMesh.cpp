#ifndef REMOVEMESH_CPP
#define REMOVEMESH_CPP

#include "pluginCommon.h"

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
	//    "<meshUUID>": null
	//  }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json configRoomPatch = nlohmann::json::object();
	nlohmann::json broadcast = nlohmann::json::object();
	nlohmann::json diff = nlohmann::json::object();
	nlohmann::json diffInv = nlohmann::json::object();

	diff["meshes"] = nlohmann::json::object();
	diffInv["meshes"] = nlohmann::json::object();
	// remove mesh
	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		diffInv.at("meshes")[meshUUID] = configRoom.at("meshes").at(meshUUID);
		diff.at("meshes")[meshUUID] = nlohmann::json(nullptr);
	}
	configRoomPatch = diff;
	broadcast = diff;

	// history
	configRoomPatch["history"] = nlohmann::json::object();
	Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
	writeJSONToChar(broadcastChar, broadcast);
}

#endif
