#ifndef PULLCURRENTMESHES_CPP
#define PULLCURRENTMESHES_CPP

#include "Doppelganger/pluginCommon.h"
#include <string>
#include <nlohmann/json.hpp>
#include "Doppelganger/TriangleMesh.h"

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/meshes");
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
	// [OUT]
	// response = {
	// 	   "meshes" : {
	//         "<meshUUID-A>": JSON object that represents the loaded mesh,
	//         "<meshUUID-B>": JSON object that represents the loaded mesh,
	//         ...
	//     }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	nlohmann::json response = nlohmann::json::object();

	// write response
	response["meshes"] = nlohmann::json::object();
	for (const auto &uuid_mesh : configRoom.at("meshes").items())
	{
		const std::string &uuid = uuid_mesh.key();
		const Doppelganger::TriangleMesh mesh = uuid_mesh.value().get<Doppelganger::TriangleMesh>();
		nlohmann::json meshJsonf;
		// we explicitly use float
		Doppelganger::to_json(meshJsonf, mesh, true);
		response.at("meshes")[uuid] = meshJsonf;
	}

	// write result
	writeJSONToChar(responseChar, response);
}

#endif
