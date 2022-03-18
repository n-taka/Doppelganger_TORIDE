#ifndef REDO_CPP
#define REDO_CPP

#include "Doppelganger/pluginCommon.h"

#include "Doppelganger/Util/filesystem.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"

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
	ptrStrArrayRoom.push_back("/history");
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
	////
	// [IN]
	// parameters = {
	// }

	// [OUT]
	// broadcast = {
	// 	   "meshes" : {
	//         "<meshUUID>": JSON object that represents the loaded mesh,
	//         ...,
	//     }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	nlohmann::json configRoomPatch = nlohmann::json::object();
	nlohmann::json broadcast = nlohmann::json::object();

	const int currentIndex = configRoom.at("history").at("index").get<int>();
	const int updatedIndex = std::min(static_cast<int>(configRoom.at("history").at("diffFromPrev").size() - 1), currentIndex + 1);

	if (updatedIndex != currentIndex)
	{
		// perform update
		configRoomPatch["history"] = configRoom.at("history");
		configRoomPatch.at("history").at("index") = updatedIndex;

		// update other parameters (if needed)
		//  * currently, we only store meshes.
		// update meshes
		configRoomPatch["meshes"] = configRoom.at("history").at("diffFromPrev").at(updatedIndex).at("meshes");

		// construct broadcast message
		broadcast["meshes"] = nlohmann::json::object();
		// we explicitly use float (this double -> float conversion could take time...)
		for (const auto &uuid_meshJson : configRoom.at("history").at("diffFromPrev").at(updatedIndex).at("meshes").items())
		{
			const std::string &uuid = uuid_meshJson.key();
			const nlohmann::json &meshJson = uuid_meshJson.value();
			if (meshJson.is_null())
			{
				// null indicates removing this mesh
				broadcast.at("meshes")[uuid] = meshJson;
			}
			else
			{
				nlohmann::json meshJsonf;
				Doppelganger::to_json(meshJsonf, meshJson.get<Doppelganger::TriangleMesh>(), true);
				broadcast.at("meshes")[uuid] = meshJsonf;
			}
		}

		// write result
		writeJSONToChar(configRoomPatchChar, configRoomPatch);
		writeJSONToChar(broadcastChar, broadcast);
	}
}

#endif
